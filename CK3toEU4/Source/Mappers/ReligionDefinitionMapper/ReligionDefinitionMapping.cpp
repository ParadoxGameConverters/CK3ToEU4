#include "ReligionDefinitionMapping.h"
#include "ParserHelpers.h"

mappers::ReligionDefinitionMapping::ReligionDefinitionMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionDefinitionMapping::registerKeys()
{
	registerKeyword("allowed_conversion", [this](const std::string& unused, std::istream& theStream) {		
		allowedConversion = commonItems::singleItem(unused, theStream);
		if (allowedConversion.size() > 1)
			allowedConversion = allowedConversion.substr(1, allowedConversion.size() - 2);
	});
	registerKeyword("country", [this](const std::string& unused, std::istream& theStream) {
		country = commonItems::singleItem(unused, theStream);
		if (country.size() > 1)
			country = country.substr(1, country.size() - 2);
	});
	registerKeyword("province", [this](const std::string& unused, std::istream& theStream) {
		province = commonItems::singleItem(unused, theStream);
		if (province.size() > 1)
			province = province.substr(1, province.size() - 2);
	});
	registerKeyword("country_as_secondary", [this](const std::string& unused, std::istream& theStream) {
		countrySecondary = commonItems::singleItem(unused, theStream);
		if (countrySecondary.size() > 1)
			countrySecondary = countrySecondary.substr(1, countrySecondary.size() - 2);
	});
	registerKeyword("unique_modifiers", [this](const std::string& unused, std::istream& theStream) {
		unique = commonItems::singleItem(unused, theStream);
		if (unique.size() > 1)
			unique = unique.substr(1, unique.size() - 2);
	});
	registerKeyword("non_unique_modifiers", [this](const std::string& unused, std::istream& theStream) {
		nonUnique = commonItems::singleItem(unused, theStream);
		if (nonUnique.size() > 1)
			nonUnique = nonUnique.substr(1, nonUnique.size() - 2);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
