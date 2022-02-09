#include "CultureMapper.h"
#include <ranges>
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::CultureMapper::CultureMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
	buildCultureCaches();
}

void mappers::CultureMapper::initializeMapper()
{
	Log(LogLevel::Info) << "-> Parsing culture mappings.";
	registerKeys();
	parseFile("configurables/culture_map.txt");
	clearRegisteredKeywords();
	buildCultureCaches();
	Log(LogLevel::Info) << "<> Loaded " << cultureMapRules.size() << " cultural links.";
}

void mappers::CultureMapper::storeCultures(const std::map<long long, std::shared_ptr<CK3::Culture>>& incCultures)
{
	for (const auto culture: incCultures | std::views::values)
	{
		if (culture->isEU4Ready())
			eu4Overrides.insert(culture->getName());
		else if (culture->isDynamic())
			eu4Overrides.insert(culture->getName());
	}
}


void mappers::CultureMapper::loadRegionMapper(const std::shared_ptr<RegionMapper>& theRegionMapper)
{
	for (auto& rule: cultureMapRules)
		rule.insertRegionMapper(theRegionMapper);
}

void mappers::CultureMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		const CultureMappingRule rule(theStream);
		cultureMapRules.push_back(rule);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::CultureMapper::cultureMatch(const std::string& ck3culture,
	 const std::string& eu4religion,
	 int eu4Province,
	 const std::string& eu4ownerTag) const
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

std::optional<std::string> mappers::CultureMapper::cultureRegionalMatch(const std::string& ck3culture,
	 const std::string& eu4religion,
	 int eu4Province,
	 const std::string& eu4ownerTag) const
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

std::optional<std::string> mappers::CultureMapper::cultureNonRegionalNonReligiousMatch(const std::string& ck3culture,
	 const std::string& eu4religion,
	 int eu4Province,
	 const std::string& eu4ownerTag) const
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

std::optional<std::string> mappers::CultureMapper::getTechGroup(const std::string& incEU4Culture) const
{
	for (const auto& mapping: cultureMapRules)
		if (mapping.getTechGroup(incEU4Culture))
			return mapping.getTechGroup(incEU4Culture);
	return std::nullopt;
}

std::optional<std::string> mappers::CultureMapper::getGFX(const std::string& incEU4Culture) const
{
	for (const auto& mapping: cultureMapRules)
		if (mapping.getGFX(incEU4Culture))
			return mapping.getGFX(incEU4Culture);
	return std::nullopt;
}

void mappers::CultureMapper::buildCultureCaches()
{
	for (const auto& mappingRule: cultureMapRules)
	{
		targetCultures.insert(mappingRule.getEU4Culture());
		for (const auto& culture: mappingRule.getCK3Cultures())
			sourceCultures.insert(culture);
	}
}
