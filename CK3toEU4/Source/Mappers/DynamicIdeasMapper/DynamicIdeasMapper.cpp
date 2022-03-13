#include "DynamicIdeasMapper.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::DynamicIdeasMapper::DynamicIdeasMapper()
{
	registerKeys();
	parseFile("configurables/tradition_ideas.txt");
	clearRegisteredKeywords();
}

void mappers::DynamicIdeasMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		const auto scraper = DynamicIdeasLink(theStream);

		if (scraper.getRules().empty())
		{
			if (auto possibleEthos = scraper.getEthos(); possibleEthos)
				ethosMap.emplace(possibleEthos.value(), scraper.getEffects());
			else if (auto possibleTradition = scraper.getTradition(); possibleTradition)
				traditionMap.emplace(possibleTradition.value(), scraper.getEffects());
			else
			{
				traditionMap.emplace(scraper.getDefault().value(), scraper.getEffects());
				defaults.push_back(scraper.getDefault().value());
			}
		}

		// Rules logic to come
		/*
		else
			rules = Rules(scraper.getRules())

		*/
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}