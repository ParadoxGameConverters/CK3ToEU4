#include "ReligionDefinitionMapping.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::ReligionDefinitionMapping::ReligionDefinitionMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionDefinitionMapping::registerKeys()
{
	registerKeyword("allowed_conversion", [this](const std::string& unused, std::istream& theStream) {		
		allowedConversion = commonItems::stringOfItem(theStream).getString();
		if (allowedConversion.size() > 4)
			allowedConversion = allowedConversion.substr(3, allowedConversion.size() - 4);
		else
			allowedConversion.clear();
	});
	registerKeyword("country", [this](const std::string& unused, std::istream& theStream) {
		country = commonItems::stringOfItem(theStream).getString();
		if (country.size() > 4)
			country = country.substr(3, country.size() - 4);
		else
			country.clear();
	});
	registerKeyword("country_as_secondary", [this](const std::string& unused, std::istream& theStream) {
		countrySecondary = commonItems::stringOfItem(theStream).getString();
		if (countrySecondary.size() > 4)
			countrySecondary = countrySecondary.substr(3, countrySecondary.size() - 4);
		else
			countrySecondary.clear();
	});
	registerKeyword("province", [this](const std::string& unused, std::istream& theStream) {
		province = commonItems::stringOfItem(theStream).getString();
		if (province.size() > 4)
			province = province.substr(3, province.size() - 4);
		else
			province.clear();
	});
	registerKeyword("unique_modifiers", [this](const std::string& unused, std::istream& theStream) {
		unique = commonItems::stringOfItem(theStream).getString();
		if (unique.size() > 4)
			unique = unique.substr(3, unique.size() - 4);
		else
			unique.clear();
	});
	registerKeyword("non_unique_modifiers", [this](const std::string& unused, std::istream& theStream) {
		nonUnique = commonItems::stringOfItem(theStream).getString();
		if (nonUnique.size() > 4)
			nonUnique = nonUnique.substr(3, nonUnique.size() - 4);
		else
			nonUnique.clear();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
