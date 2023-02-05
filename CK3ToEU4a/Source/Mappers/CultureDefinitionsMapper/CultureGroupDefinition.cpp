#include "CultureGroupDefinition.h"
#include "CommonRegexes.h"
#include "CultureDefiniton.h"
#include "ParserHelpers.h"

mappers::CultureGroupDefinition::CultureGroupDefinition(std::string theName, std::istream& theStream): name(std::move(theName))
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CultureGroupDefinition::registerKeys()
{
	registerRegex("second_graphical_culture|graphical_culture", [this](const std::string& unused, std::istream& theStream) {
		commonItems::ignoreItem(unused, theStream);
	});
	registerKeyword("male_names", [this](std::istream& theStream) {
		maleNames = commonItems::getStrings(theStream);
	});
	registerKeyword("female_names", [this](std::istream& theStream) {
		femaleNames = commonItems::getStrings(theStream);
	});
	registerKeyword("dynasty_names", [this](std::istream& theStream) {
		dynastyNames = commonItems::getStrings(theStream);
	});
	registerRegex(R"([\w_]+)", [this](std::string cultureName, std::istream& theStream) {
		auto newCulture = std::make_shared<CultureDefinition>(theStream);
		cultures.insert(std::make_pair(cultureName, newCulture));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void mappers::CultureGroupDefinition::mergeCulture(const std::string& theName, const std::shared_ptr<CultureDefinition>& culture)
{
	if (const auto& cultureItr = cultures.find(theName); cultureItr != cultures.end())
	{
		cultureItr->second->addMaleNames(culture->getMaleNames());
		cultureItr->second->addFemaleNames(culture->getFemaleNames());
		cultureItr->second->addDynastyNames(culture->getDynastyNames());
		if (!cultureItr->second->getSourceCulture()) // don't override original pointer if there's one.
			cultureItr->second->setSourceCulture(culture->getSourceCulture());
	}
	else
	{
		cultures.emplace(theName, culture);
	}
}
