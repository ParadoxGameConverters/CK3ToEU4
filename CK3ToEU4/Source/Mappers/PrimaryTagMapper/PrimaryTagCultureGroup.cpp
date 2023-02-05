#include "PrimaryTagCultureGroup.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include "PrimaryTagCulture.h"

mappers::PrimaryTagCultureGroup::PrimaryTagCultureGroup(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::PrimaryTagCultureGroup::registerKeys()
{
	registerKeyword("graphical_culture", commonItems::ignoreItem);
	registerKeyword("second_graphical_culture", commonItems::ignoreItem);
	registerKeyword("unit", commonItems::ignoreItem);
	registerKeyword("leader", commonItems::ignoreItem);
	registerKeyword("is_overseas", commonItems::ignoreItem);
	registerKeyword("union", commonItems::ignoreItem);
	registerKeyword("male_names", commonItems::ignoreItem);
	registerKeyword("female_names", commonItems::ignoreItem);
	registerKeyword("dynasty_names", commonItems::ignoreItem);
	registerRegex(commonItems::catchallRegex, [this](const std::string& culture, std::istream& theStream) {
		const PrimaryTagCulture theCulture(theStream);
		const auto& theTag = theCulture.getTag();
		if (theTag)
			cultureTags.insert(std::pair(culture, *theTag));
	});
}
