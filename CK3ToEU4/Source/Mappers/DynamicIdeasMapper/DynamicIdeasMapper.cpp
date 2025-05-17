#include "DynamicIdeasMapper.h"
#include "../../EU4World/EU4World.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include <ranges>

mappers::DynamicIdeasMapper::DynamicIdeasMapper(const LocalizationMapper& localizationMapper): locs(localizationMapper)
{
	registerKeys();
	parseFile(std::filesystem::path("configurables/tradition_ideas.txt"));
	clearRegisteredKeywords();
	processRules();
}
mappers::DynamicIdeasMapper::DynamicIdeasMapper(std::istream& theStream, const LocalizationMapper& localizationMapper): locs(localizationMapper)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
	processRules();
}

void mappers::DynamicIdeasMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		const auto scraper = DynamicIdeasLink(theStream);

		const auto& newRules = scraper.getRules();
		const auto& effects = scraper.getEffects();

		if (newRules.empty()) // Is a base mapping
		{
			if (auto possibleTradition = scraper.getTradition(); possibleTradition)
				traditionMap.emplace(possibleTradition.value(), effects);
			else if (auto possibleEthos = scraper.getEthos(); possibleEthos)
				ethosMap.emplace(possibleEthos.value(), effects);
			else if (auto possibleDefault = scraper.getDefault())
			{
				traditionMap.emplace(scraper.getDefault().value(), effects);
				defaults.push_back(scraper.getDefault().value());
			}
			else
				Log(LogLevel::Warning) << "A link was skipped due to invalid input.";
		}
		else // Is a rule based override mapping
		{
			if (auto possibleTradition = scraper.getTradition(); possibleTradition)
				rules.emplace(DynamicIdeasRule(newRules, effects, possibleTradition.value(), scraper.getIdeaName()));
			else if (auto possibleEthos = scraper.getEthos(); possibleEthos)
				rules.emplace(DynamicIdeasRule(newRules, effects, possibleEthos.value(), scraper.getIdeaName()));
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void mappers::DynamicIdeasMapper::processRules()
{
	// Add rules to tradition map
	for (const auto& rule: rules)
		if (rule.getReplacee().find("tradition") != std::string::npos)
			traditionMap.emplace(rule.getReplacement(), rule.getNewEffect());

	// NOTE: Nested localizationd DO NOT work for EU4.
	// Add localizations for every tradition

	for (auto& tradition: traditionMap | std::views::keys)
	{
		const auto& key = getLeadStr(tradition, 1, "__");
		if (const auto& block = locs.getLocBlockForKey(key + "_name"); block)
		{
			LocBlock copyblock = block.value();
			locs.unravelNestedLocs(copyblock);
			traditionLocs.emplace(tradition, copyblock);
		}
		if (const auto& block = locs.getLocBlockForKey(key + "_desc"); block)
		{
			LocBlock copyblock = block.value();
			locs.unravelNestedLocs(copyblock);
			locs.removeEmphasis(copyblock);
			traditionLocs.emplace(tradition + "_desc", copyblock);
		}
	}
}