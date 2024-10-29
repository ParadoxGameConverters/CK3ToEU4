
using CK3ToEU4.Mappers.CultureMapper;

namespace mappers;

class CultureMapper
{
	public CultureMapper() = default;
	public CultureMapper(std::istream& theStream)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
		buildCultureCaches();
	}
	public void loadCulturesFromDisk()
	{
		Log(LogLevel::Info) << "-> Parsing culture mappings.";
		registerKeys();
		parseFile("configurables/culture_map.txt");
		clearRegisteredKeywords();
		buildCultureCaches();
		Log(LogLevel::Info) << "<> Loaded " << cultureMapRules.size() << " cultural links.";
	}

	public void loadRegionMapper(const std::shared_ptr<RegionMapper>& theRegionMapper)
	{
		for (auto& rule: cultureMapRules)
		rule.insertRegionMapper(theRegionMapper);
	}
	public void storeCultures(const std::set<std::shared_ptr<CK3::Culture>>& incCultures)
	{
		for (const auto culture: incCultures)
		{
			if (culture->isEU4Ready())
				eu4Overrides.insert(culture->getName());
			else if (culture->isDynamic())
				eu4Overrides.insert(culture->getName());
		}
		cultures = incCultures;
	}

	public std::optional<std::string> cultureMatch(const std::string& ck3culture,
	const std::string& eu4religion,
	int eu4Province,
	const std::string& eu4ownerTag)
	{
		if (eu4Overrides.contains(ck3culture))
			return ck3culture;

		for (const auto& cultureMappingRule: cultureMapRules)
		{
			const auto& possibleMatch = cultureMappingRule.cultureMatch(ck3culture, eu4religion, eu4Province, eu4ownerTag);
			if (possibleMatch)
				return *possibleMatch;
		}
		return std::nullopt;
	}

	public std::optional<std::string> cultureRegionalMatch(const std::string& ck3culture,
	const std::string& eu4religion,
	int eu4Province,
	const std::string& eu4ownerTag)
	{
		if (eu4Overrides.contains(ck3culture))
			return ck3culture;

		for (const auto& cultureMappingRule: cultureMapRules)
		{
			const auto& possibleMatch = cultureMappingRule.cultureRegionalMatch(ck3culture, eu4religion, eu4Province, eu4ownerTag);
			if (possibleMatch)
				return *possibleMatch;
		}
		return std::nullopt;
	}

	public std::optional<std::string> cultureNonRegionalNonReligiousMatch(const std::string& ck3culture,
	const std::string& eu4religion,
	int eu4Province,
	const std::string& eu4ownerTag)
	{
		if (eu4Overrides.contains(ck3culture))
			return ck3culture;

		for (const auto& cultureMappingRule: cultureMapRules)
		{
			const auto& possibleMatch = cultureMappingRule.cultureNonRegionalNonReligiousMatch(ck3culture, eu4religion, eu4Province, eu4ownerTag);
			if (possibleMatch)
				return *possibleMatch;
		}
		return std::nullopt;
	}

	public std::optional<std::string> getTechGroup(const std::string& incEU4Culture)
	{
		auto checkCulture = incEU4Culture;
		if (eu4Overrides.contains(incEU4Culture))
		{
			// if this is dynamic (not eu4ready) then see if we can grab a nonregional nonreligious mapping and ping the results of that.
			for (const auto& culture: cultures)
			{
				// Log(LogLevel::Debug) << "> vs: " << culture->getName() << " | " << culture->isDynamic() << " | " << culture->getNameList().empty();

				if (culture->getName() == incEU4Culture && culture->isDynamic() && !culture->getNameLists().empty())
				{
					const auto& origCulture = *culture->getNameLists().begin();
					const auto& match = cultureNonRegionalNonReligiousMatch(origCulture, "", 0, "");
					if (match)
						checkCulture = *match;
				}
			}
		}

		for (const auto& mapping: cultureMapRules)
		if (mapping.getTechGroup(checkCulture))
			return mapping.getTechGroup(checkCulture);
		return std::nullopt;
	}
	public std::optional<std::string> getGFX(const std::string& incEU4Culture)
	{
		auto checkCulture = incEU4Culture;
		if (eu4Overrides.contains(incEU4Culture))
		{
			// if this is dynamic (not eu4ready) then see if we can grab a nonregional nonreligious mapping and ping the results of that.
			for (const auto& culture: cultures)
			{
				if (culture->getName() == incEU4Culture && culture->isDynamic() && !culture->getNameLists().empty())
				{
					const auto& origCulture = *culture->getNameLists().begin();
					const auto& match = cultureNonRegionalNonReligiousMatch(origCulture, "", 0, "");
					if (match)
						checkCulture = *match;
				}
			}
		}

		for (const auto& mapping: cultureMapRules)
		if (mapping.getGFX(checkCulture))
			return mapping.getGFX(checkCulture);
		return std::nullopt;
	}

	public const auto& getTargetCultures() const { return targetCultures; }
	public const auto& getSourceCultures() const { return sourceCultures; }
	public const auto& getCK3Cultures() const { return cultures; }

	private void registerKeys()
	{
		registerKeyword("link", [this](std::istream& theStream) {
			const CultureMappingRule rule(theStream);
			cultureMapRules.push_back(rule);
		});
		registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	}
	private void buildCultureCaches()
	{
		for (const auto& mappingRule: cultureMapRules)
		{
			targetCultures.insert(mappingRule.getEU4Culture());
			for (const auto& culture: mappingRule.getCK3Cultures())
			sourceCultures.insert(culture);
		}
	}


	private std::vector<CultureMappingRule> cultureMapRules;

	private std::set<std::string> targetCultures;
	private std::set<std::string> sourceCultures;
	private std::set<std::shared_ptr<CK3::Culture>> cultures;

	private std::set<std::string> eu4Overrides; // cultures from ck3 we don't map but return as is - either eu4 ready cultures or dynamic ones.
};