#include "DynamicIdeasMapper.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::DynamicIdeasMapper::DynamicIdeasMapper()
{
	registerKeys();
	parseFile("configurables/tradition_ideas.txt");
	clearRegisteredKeywords();

	for (const auto& rule: rules)
		if (rule.getReplacee().contains("tradition"))
			traditionMap.emplace(rule.getReplacement(), rule.getNewEffect());
}
mappers::DynamicIdeasMapper::DynamicIdeasMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	for (const auto& rule: rules)
		if (rule.getReplacee().contains("tradition"))
			traditionMap.emplace(rule.getReplacement(), rule.getNewEffect());
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
			else
			{
				traditionMap.emplace(scraper.getDefault().value(), effects);
				defaults.push_back(scraper.getDefault().value());
			}
		}
		else // Is a rule based override mapping
		{
			if (auto possibleTradition = scraper.getTradition(); possibleTradition)
				rules.emplace(DynamicIdeasRule(newRules, possibleTradition.value(), effects));
			else if (auto possibleEthos = scraper.getEthos(); possibleEthos)
				rules.emplace(DynamicIdeasRule(newRules, possibleEthos.value(), effects));
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}