#include "CharacterDomain.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::CharacterDomain::CharacterDomain(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::CharacterDomain::registerKeys()
{
	registerKeyword("is_powerful_vassal", [this](const std::string& unused, std::istream& theStream) {
		powerfulVassal = commonItems::singleString(theStream).getString() == "yes";
	});
	registerKeyword("vassal_power_value", [this](const std::string& unused, std::istream& theStream) {
		vassalStrength = commonItems::singleDouble(theStream).getDouble();
	});
	registerKeyword("government", [this](const std::string& unused, std::istream& theStream) {
		government = commonItems::singleString(theStream).getString();
	});
	registerKeyword("laws", [this](const std::string& unused, std::istream& theStream) {
		const auto& lawsList = commonItems::stringList(theStream).getStrings();
		laws = std::set(lawsList.begin(), lawsList.end());
	});
	registerKeyword("realm_capital", [this](const std::string& unused, std::istream& theStream) {
		realmCapital = std::make_pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
	});
	registerKeyword("domain", [this](const std::string& unused, std::istream& theStream) {
		for (auto titleID: commonItems::llongList(theStream).getLlongs())
			domain.emplace_back(std::pair(titleID, nullptr));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
