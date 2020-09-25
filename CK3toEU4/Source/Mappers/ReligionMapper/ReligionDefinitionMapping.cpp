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
	registerKeyword("country", [this](const std::string& unused, std::istream& theStream) {		
		country = commonItems::singleItem(unused, theStream);
		if (country.length() > 1)
			country = country.substr(1, country.length()-2);
	});
	registerKeyword("province", [this](const std::string& unused, std::istream& theStream) {
		province = commonItems::singleItem(unused, theStream);
		if (province.length() > 1)
			province = province.substr(1, province.length() - 2);
	});
	registerKeyword("country_as_secondary", [this](const std::string& unused, std::istream& theStream) {
		countrySecondary = commonItems::singleItem(unused, theStream);
		if (countrySecondary.length() > 1)
			countrySecondary = countrySecondary.substr(1, countrySecondary.length() - 2);
	});
	registerKeyword("unique_modifiers", [this](const std::string& unused, std::istream& theStream) {
		unique = commonItems::singleItem(unused, theStream);
		if (unique.length() > 1)
			unique = unique.substr(1, unique.length() - 2);
	});
	registerKeyword("non_unique_modifiers", [this](const std::string& unused, std::istream& theStream) {
		nonUnique = commonItems::singleItem(unused, theStream);
		if (nonUnique.length() > 1)
			nonUnique = nonUnique.substr(1, nonUnique.length() - 2);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
