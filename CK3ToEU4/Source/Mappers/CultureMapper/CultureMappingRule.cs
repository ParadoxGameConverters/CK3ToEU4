namespace CK3ToEU4.Mappers.CultureMapper;

class CultureMappingRule
{
	public CultureMappingRule() = default;
	public CultureMappingRule(std::istream& theStream)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}

	public std::optional<std::string> cultureMatch(const std::string& ck3culture,
	const std::string& eu4religion,
	int eu4Province,
	const std::string& eu4ownerTag) 
	{
		// We need at least a viable EU4culture.
		if (ck3culture.empty())
			return std::nullopt;

		if (!cultures.contains(ck3culture))
			return std::nullopt;

		if (!owners.empty())
			if (eu4ownerTag.empty() || !owners.contains(eu4ownerTag))
				return std::nullopt;

		if (!religions.empty())
			if (eu4religion.empty() || !religions.contains(eu4religion))
				return std::nullopt;

		// This is a straight province check, not regions.
		if (!provinces.empty() && regions.empty())
			if (!eu4Province || !provinces.contains(eu4Province))
				return std::nullopt;

		// Asking for a regions check without an incoming province is pointless.
		if (!regions.empty() && !eu4Province)
			return std::nullopt;

		// This is a regions check, that checks if a provided province is within that region.
		if (eu4Province && !regions.empty())
		{
			if (!regionMapper)
				throw std::runtime_error("Culture Mapper: Region Mapper is unloaded!");
			auto regionMatch = false;
			for (const auto& region: regions)
			{
				if (!regionMapper->regionNameIsValid(region))
				{
					Log(LogLevel::Warning) << "Checking for culture " << ck3culture << " inside invalid region: " << region << "! Fix the mapping rules!";
					// We could say this was a match, and thus pretend this region entry doesn't exist, but it's better
					// for the converter to explode across the logs with invalid names. So, continue.
					continue;
				}
				if (regionMapper->provinceIsInRegion(eu4Province, region))
					regionMatch = true;
			}
			// This is an override if we have a province outside the regions specified.
			if (!provinces.empty() && provinces.count(eu4Province))
				regionMatch = true;
			if (!regionMatch)
				return std::nullopt;
		}
		return destinationCulture;
	}

	public std::optional<std::string> cultureRegionalMatch(const std::string& ck3culture,
	const std::string& eu4religion,
	int eu4Province,
	const std::string& eu4ownerTag)
	{
		// This is a regional match. We need a mapping within the given region, so if the
		// mapping rule has no regional qualifiers it needs to fail.
		if (regions.empty())
			return std::nullopt;

		// Otherwise, as usual.
		return cultureMatch(ck3culture, eu4religion, eu4Province, eu4ownerTag);
	}

	public std::optional<std::string> cultureNonRegionalNonReligiousMatch(const std::string& ck3culture,
	const std::string& eu4religion,
	int eu4Province,
	const std::string& eu4ownerTag) 
	{
		// This is a non regional non religious match. We need a mapping without any region/religion, so if the
		// mapping rule has any regional/religious qualifiers it needs to fail.
		if (!regions.empty())
			return std::nullopt;
		if (!religions.empty())
			return std::nullopt;

		// Otherwise, as usual.
		return cultureMatch(ck3culture, eu4religion, eu4Province, eu4ownerTag);
	}

	public std::optional<std::string> getTechGroup(const std::string& incEU4Culture)
	{
		if (incEU4Culture == destinationCulture && !techGroup.empty())
			return techGroup;
		return std::nullopt;
	}
	public std::optional<std::string> getGFX(const std::string& incEU4Culture)
	{
		if (incEU4Culture == destinationCulture && !gfx.empty())
			return gfx;
		return std::nullopt;
	}

	public void insertRegionMapper(const std::shared_ptr<RegionMapper>& theRegionMapper) { regionMapper = theRegionMapper; }

	public const auto& getEU4Culture() const { return destinationCulture; } // for testing
	public const auto& getCK3Cultures() const { return cultures; }			 // for testing
	public const auto& getReligions() const { return religions; }				 // for testing
	public const auto& getRegions() const { return regions; }					 // for testing
	public const auto& getOwners() const { return owners; }						 // for testing
	public const auto& getProvinces() const { return provinces; }				 // for testing
	public const auto& getTechGroupField() const { return techGroup; }		 // for testing
	public const auto& getGFXField() const { return gfx; }						 // for testing

	private void registerKeys()
	{
		registerKeyword("eu4", [this](const std::string& unused, std::istream& theStream) {
			const commonItems::singleString eu4Str(theStream);
			destinationCulture = eu4Str.getString();
		});
		registerKeyword("region", [this](const std::string& unused, std::istream& theStream) {
			const commonItems::singleString regionStr(theStream);
			regions.insert(regionStr.getString());
		});
		registerKeyword("religion", [this](const std::string& unused, std::istream& theStream) {
			const commonItems::singleString religionStr(theStream);
			religions.insert(religionStr.getString());
		});
		registerKeyword("owner", [this](const std::string& unused, std::istream& theStream) {
			const commonItems::singleString ownerStr(theStream);
			owners.insert(ownerStr.getString());
		});
		registerKeyword("tech", [this](const std::string& unused, std::istream& theStream) {
			const commonItems::singleString techStr(theStream);
			techGroup = techStr.getString();
		});
		registerKeyword("gfx", [this](const std::string& unused, std::istream& theStream) {
			const commonItems::singleString gfxStr(theStream);
			gfx = gfxStr.getString();
		});
		registerKeyword("province", [this](const std::string& unused, std::istream& theStream) {
			const commonItems::singleString provinceStr(theStream);
			try
			{
				provinces.insert(stoi(provinceStr.getString()));
			}
			catch (std::exception&)
			{
				Log(LogLevel::Warning) << "Invalid province ID in culture mapper: " << provinceStr.getString();
			}
		});
		registerKeyword("ck3", [this](const std::string& unused, std::istream& theStream) {
			const commonItems::singleString ck3Str(theStream);
			cultures.insert(ck3Str.getString());
		});
		registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	}

	private std::string destinationCulture;
	private std::string techGroup;
	private std::string gfx;
	private std::set<std::string> cultures;
	private std::set<std::string> religions;
	private std::set<std::string> regions;
	private std::set<std::string> owners;
	private std::set<int> provinces;

	private std::shared_ptr<RegionMapper> regionMapper;
}