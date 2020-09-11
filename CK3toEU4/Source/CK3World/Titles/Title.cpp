#include "Title.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Title::Title(std::istream& theStream, int ID): ID(ID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Title::registerKeys()
{
	registerKeyword("key", [this](const std::string& unused, std::istream& theStream) {
		name = commonItems::singleString(theStream).getString();
	});
	registerKeyword("name", [this](const std::string& unused, std::istream& theStream) {
		displayName = commonItems::singleString(theStream).getString();
	});
	registerKeyword("adj", [this](const std::string& unused, std::istream& theStream) {
		adjective = commonItems::singleString(theStream).getString();
	});
	registerKeyword("date", [this](const std::string& unused, std::istream& theStream) {
		creationDate = date(commonItems::singleString(theStream).getString());
	});
	registerKeyword("history_government", [this](const std::string& unused, std::istream& theStream) {
		historyGovernment = commonItems::singleString(theStream).getString();
	});
	registerKeyword("theocratic_lease", [this](const std::string& unused, std::istream& theStream) {
		theocraticLease = commonItems::singleString(theStream).getString() == "yes";
	});
	registerKeyword("capital_barony", [this](const std::string& unused, std::istream& theStream) {
		cCapitalBarony = commonItems::singleString(theStream).getString() == "yes";
	});
	registerKeyword("duchy_capital_barony", [this](const std::string& unused, std::istream& theStream) {
		dCapitalBarony = commonItems::singleString(theStream).getString() == "yes";
	});
	registerKeyword("capital", [this](const std::string& unused, std::istream& theStream) {
		capital = std::pair(commonItems::singleInt(theStream).getInt(), nullptr);
	});
	registerKeyword("de_facto_liege", [this](const std::string& unused, std::istream& theStream) {
		dfLiege = std::pair(commonItems::singleInt(theStream).getInt(), nullptr);
	});
	registerKeyword("de_jure_liege", [this](const std::string& unused, std::istream& theStream) {
		djLiege = std::pair(commonItems::singleInt(theStream).getInt(), nullptr);
	});
	registerKeyword("de_jure_vassals", [this](const std::string& unused, std::istream& theStream) {
		for (const auto& vassalID: commonItems::intList(theStream).getInts())
		{
			djVassals.emplace(std::pair(vassalID, nullptr));
		}
	});
	registerKeyword("heir", [this](const std::string& unused, std::istream& theStream) {
		for (const auto& heirID: commonItems::intList(theStream).getInts())
		{
			heirs.emplace_back(std::pair(heirID, nullptr));
		}
	});
	registerKeyword("laws", [this](const std::string& unused, std::istream& theStream) {
		const auto& theLaws = commonItems::stringList(theStream).getStrings();
		laws = std::set(theLaws.begin(), theLaws.end());
	});
	registerKeyword("holder", [this](const std::string& unused, std::istream& theStream) {
		holder = std::pair(commonItems::singleInt(theStream).getInt(), nullptr);
	});
	registerKeyword("coat_of_arms_id", [this](const std::string& unused, std::istream& theStream) {
		coa = std::pair(commonItems::singleInt(theStream).getInt(), nullptr);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
