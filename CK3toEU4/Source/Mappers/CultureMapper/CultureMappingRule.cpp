#include "CultureMappingRule.h"
#include "../RegionMapper/RegionMapper.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::CultureMappingRule::CultureMappingRule(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CultureMappingRule::registerKeys()
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


std::optional<std::string> mappers::CultureMappingRule::cultureMatch(const std::string& ck3culture,
	 const std::string& eu4religion,
	 int eu4Province,
	 const std::string& eu4ownerTag) const
{
	// We need at least a viable EU4culture.
	if (ck3culture.empty())
		return std::nullopt;

	if (!cultures.count(ck3culture))
		return std::nullopt;

	if (!owners.empty())
		if (eu4ownerTag.empty() || !owners.count(eu4ownerTag))
			return std::nullopt;

	if (!religions.empty())
		if (eu4religion.empty() || !religions.count(eu4religion))
			return std::nullopt;

	// This is a straight province check, not regions.
	if (!provinces.empty() && regions.empty())
		if (!eu4Province || !provinces.count(eu4Province))
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

std::optional<std::string> mappers::CultureMappingRule::cultureRegionalMatch(const std::string& ck3culture,
	 const std::string& eu4religion,
	 int eu4Province,
	 const std::string& eu4ownerTag) const
{
	// This is a regional match. We need a mapping within the given region, so if the
	// mapping rule has no regional qualifiers it needs to fail.
	if (regions.empty())
		return std::nullopt;

	// Otherwise, as usual.
	return cultureMatch(ck3culture, eu4religion, eu4Province, eu4ownerTag);
}

std::optional<std::string> mappers::CultureMappingRule::cultureNonRegionalNonReligiousMatch(const std::string& ck3culture,
	 const std::string& eu4religion,
	 int eu4Province,
	 const std::string& eu4ownerTag) const
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

std::optional<std::string> mappers::CultureMappingRule::getTechGroup(const std::string& incEU4Culture) const
{
	if (incEU4Culture == destinationCulture && !techGroup.empty())
		return techGroup;
	return std::nullopt;
}

std::optional<std::string> mappers::CultureMappingRule::getGFX(const std::string& incEU4Culture) const
{
	if (incEU4Culture == destinationCulture && !gfx.empty())
		return gfx;
	return std::nullopt;
}
