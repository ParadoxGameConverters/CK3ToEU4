#include "TraitScraper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "TraitScraping.h"

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
		if (newScraping.getIndex() > 0) // indexes start at 1!
		{
			if (traits.contains(newScraping.getIndex()))
			{
				traits.at(newScraping.getIndex()) = traitName; // override existing one
			}
			else
			{
				traits.emplace(newScraping.getIndex(), traitName);
			}
		}
		else // otherwise index them ourselves.
		{
			if (!traits.contains(index))
			{
				traits.emplace(index, traitName);
			}
			else
			{
				traits.at(index) = traitName; // override existing one
			}
			++index;
		}
	});
}

std::optional<std::string> mappers::TraitScraper::getTraitForID(int ID) const
{
	const auto& traitsItr = traits.find(ID);
	if (traitsItr != traits.end())
		return traitsItr->second;
	return std::nullopt;
}
