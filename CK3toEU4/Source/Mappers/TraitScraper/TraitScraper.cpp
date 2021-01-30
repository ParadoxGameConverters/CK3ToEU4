#include "TraitScraper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "TraitScraping.h"
#include "CommonRegexes.h"

void mappers::TraitScraper::loadTraits(const std::string& fileName)
{
	registerKeys();
	parseFile(fileName);
	clearRegisteredKeywords();
}

void mappers::TraitScraper::loadTraits(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::TraitScraper::registerKeys()
{
	registerRegex("@.*", commonItems::ignoreItem);
	registerRegex(commonItems::catchallRegex, [this](const std::string& traitName, std::istream& theStream) {
		const auto newScraping = TraitScraping(theStream);
		// we're overriding previously loaded trait names.
		if (newScraping.getIndex() > 0) // indexes start at 1!
			traits[newScraping.getIndex()] = traitName;
	});
}

std::optional<std::string> mappers::TraitScraper::getTraitForID(int ID) const
{
	const auto& traitsItr = traits.find(ID);
	if (traitsItr != traits.end())
		return traitsItr->second;
	return std::nullopt;
}
