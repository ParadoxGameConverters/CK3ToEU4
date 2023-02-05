#include "CultureDefiniton.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::CultureDefinition::CultureDefinition(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CultureDefinition::registerKeys()
{
	registerKeyword("male_names", [this](std::istream& theStream) {
		maleNames = commonItems::getStrings(theStream);
	});
	registerKeyword("female_names", [this](std::istream& theStream) {
		femaleNames = commonItems::getStrings(theStream);
	});
	registerKeyword("dynasty_names", [this](std::istream& theStream) {
		dynastyNames = commonItems::getStrings(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
