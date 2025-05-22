#include "CountryDetails.h"
#include "CommonRegexes.h"
#include "MonarchNames.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"

EU4::CountryDetails::CountryDetails(const std::filesystem::path& filePath)
{
	registerKeys();
	if (commonItems::DoesFileExist(filePath))
	{
		parseFile(filePath);
	}
	clearRegisteredKeywords();
}

EU4::CountryDetails::CountryDetails(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::CountryDetails::parseHistory(const std::filesystem::path& filePath)
{
	registerHistoryKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
}

void EU4::CountryDetails::parseHistory(std::istream& theStream)
{
	registerHistoryKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::CountryDetails::registerKeys()
{
	registerKeyword("graphical_culture", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString graphStr(theStream);
		graphicalCulture = graphStr.getString();
	});
	registerKeyword("color", [this](const std::string& unused, std::istream& theStream) {
		color = commonItems::Color::Factory{}.getColor(theStream);
	});
	registerKeyword("revolutionary_colors", [this](const std::string& unused, std::istream& theStream) {
		revolutionaryColor = commonItems::Color::Factory{}.getColor(theStream);
	});
	registerKeyword("historical_idea_groups", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::stringList ideaList(theStream);
		historicalIdeaGroups = ideaList.getStrings();
	});
	registerKeyword("historical_score", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt scoreInt(theStream);
		historicalScore = scoreInt.getInt();
	});
	registerKeyword("random_nation_chance", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt chanceInt(theStream);
		randomChance = chanceInt.getInt() == 0;
	});
	registerKeyword("historical_units", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::stringList unitsList(theStream);
		const auto& theIdeas = unitsList.getStrings();
		historicalUnits.insert(theIdeas.begin(), theIdeas.end());
	});
	registerKeyword("monarch_names", [this](const std::string& unused, std::istream& theStream) {
		const auto mNames = MonarchNames(theStream);
		monarchNames = mNames.getMonarchNames();
	});
	registerKeyword("leader_names", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::stringList namesList(theStream);
		const auto& theNames = namesList.getStrings();
		leaderNames.insert(theNames.begin(), theNames.end());
	});
	registerKeyword("ship_names", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::stringList namesList(theStream);
		const auto& theNames = namesList.getStrings();
		shipNames.insert(theNames.begin(), theNames.end());
	});
	registerKeyword("army_names", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::stringList namesList(theStream);
		const auto& theNames = namesList.getStrings();
		armyNames.insert(theNames.begin(), theNames.end());
	});
	registerKeyword("fleet_names", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::stringList namesList(theStream);
		const auto& theNames = namesList.getStrings();
		fleetNames.insert(theNames.begin(), theNames.end());
	});
	registerKeyword("preferred_religion", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString religStr(theStream);
		preferredReligion = religStr.getString();
	});
	registerKeyword("colonial_parent", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString parentStr(theStream);
		colonialParent = parentStr.getString();
	});
	registerKeyword("special_unit_culture", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString unitStr(theStream);
		specialUnitCulture = unitStr.getString();
	});
	registerKeyword("all_your_core_are_belong_to_us", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString synStr(theStream);
		all_your_core_are_belong_to_us = synStr.getString() == "yes";
	});
	registerKeyword("right_to_bear_arms", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString mayenStr(theStream);
		rightToBEARArms = mayenStr.getString() == "yes";
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void EU4::CountryDetails::registerHistoryKeys()
{
	registerKeyword("government", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString govStr(theStream);
		government = govStr.getString();
	});
	registerKeyword("add_government_reform", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString reformStr(theStream);
		reforms.clear();
		reforms.insert(reformStr.getString());
	});
	registerKeyword("government_rank", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt rankInt(theStream);
		governmentRank = rankInt.getInt();
	});
	registerKeyword("technology_group", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString techStr(theStream);
		technologyGroup = techStr.getString();
	});
	registerKeyword("religion", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString religionStr(theStream);
		religion = religionStr.getString();
	});
	registerKeyword("primary_culture", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString culStr(theStream);
		primaryCulture = culStr.getString();
	});
	registerKeyword("capital", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt capitalInt(theStream);
		capital = capitalInt.getInt();
	});
	registerKeyword("fixed_capital", [this](const std::string& unused, std::istream& theStream) {
		commonItems::ignoreItem(unused, theStream);
		fixedCapital = true;
	});
	registerKeyword("mercantilism", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt mercInt(theStream);
		mercantilism = mercInt.getInt();
	});
	registerKeyword("add_adm_tech", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt aat(theStream);
		addedAdminTech = aat.getInt();
	});
	registerKeyword("add_dip_tech", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt adt(theStream);
		addedDipTech = adt.getInt();
	});
	registerKeyword("add_mil_tech", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt amt(theStream);
		addedMilTech = amt.getInt();
	});
	registerKeyword("unit_type", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString unitsStr(theStream);
		unitType = unitsStr.getString();
	});
	registerKeyword("religious_school", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString schoolStr(theStream);
		religiousSchool = schoolStr.getString();
	});
	registerKeyword("unlock_cult", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString cultStr(theStream);
		cults.insert(cultStr.getString());
	});
	registerKeyword("add_accepted_culture", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString cultureStr(theStream);
		acceptedCultures.insert(cultureStr.getString());
	});
	registerKeyword("add_army_professionalism", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleDouble armyDbl(theStream);
		armyProfessionalism = armyDbl.getDouble();
	});
	registerKeyword("historical_rival", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString rivalStr(theStream);
		historicalRivals.insert(rivalStr.getString());
	});
	registerKeyword("historical_friend", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString friendStr(theStream);
		historicalFriends.insert(friendStr.getString());
	});
	registerKeyword("national_focus", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString focusStr(theStream);
		nationalFocus = focusStr.getString();
	});
	registerKeyword("add_piety", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleDouble pietyDbl(theStream);
		piety = pietyDbl.getDouble();
	});
	registerKeyword("elector", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString electorStr(theStream);
		elector = electorStr.getString() == "yes";
	});
	registerKeyword("secondary_religion", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString rel2Str(theStream);
		secondaryReligion = rel2Str.getString();
	});
	registerKeyword("add_harmonized_religion", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString rel3Str(theStream);
		harmonizedReligions.insert(rel3Str.getString());
	});
	registerRegex(R"(\d+.\d+.\d+)", [this](const std::string& theDate, std::istream& theStream) {
		auto historyItem = commonItems::stringOfItem(theStream).getString();
		historyLessons.emplace_back(std::pair(date(theDate), historyItem));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
