#include "DynamicIdeasMapper.h"
#include "DynamicIdeasLink.h"
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

		if (!scraper.getRules().empty())
		{
			if (scraper.getEthos())
				ethosMap.emplace(scraper.getEthos().value(), scraper.getEffects());
			else if (scraper.getTradition())
				traditionMap.emplace(scraper.getTradition().value(), scraper.getEffects());
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