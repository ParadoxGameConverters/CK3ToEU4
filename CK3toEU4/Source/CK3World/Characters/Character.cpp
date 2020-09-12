#include "Character.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Character::Character(std::istream& theStream, int charID): charID(charID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Character::registerKeys()
{
	registerKeyword("first_name", [this](const std::string& unused, std::istream& theStream) {
		name = commonItems::singleString(theStream).getString();
	});
	registerKeyword("birth", [this](const std::string& unused, std::istream& theStream) {
		birthDate = date(commonItems::singleString(theStream).getString());
	});
	registerKeyword("culture", [this](const std::string& unused, std::istream& theStream) {
		culture = std::pair(commonItems::singleInt(theStream).getInt(), nullptr);
	});
	registerKeyword("faith", [this](const std::string& unused, std::istream& theStream) {
		faith = std::pair(commonItems::singleInt(theStream).getInt(), nullptr);
	});
	registerKeyword("dynasty_house", [this](const std::string& unused, std::istream& theStream) {
		house = std::pair(commonItems::singleInt(theStream).getInt(), nullptr);
	});
	registerKeyword("skill", [this](const std::string& unused, std::istream& theStream) {
		const auto& skillList = commonItems::intList(theStream).getInts();
		if (skillList.size() > 5)
		{
			skills.diplomacy = skillList[0];
			skills.martial = skillList[1];
			skills.stewardship = skillList[2];
			skills.intrigue = skillList[3];
			skills.learning = skillList[4];
		}
		else
			Log(LogLevel::Error) << "Character " << charID << " has a malformed skills block! Size: " << skillList.size();
	});
	registerKeyword("traits", [this](const std::string& unused, std::istream& theStream) {
		for (const auto traitID: commonItems::intList(theStream).getInts())
			traits.insert(std::pair(traitID, std::string()));
	});
	registerKeyword("alive_data", [this](const std::string& unused, std::istream& theStream) {
		const auto tempBlock = Character(theStream, charID);
		piety = tempBlock.getPiety();
		prestige = tempBlock.getPrestige();
		gold = tempBlock.getGold();
		claims = tempBlock.getClaims();
	});
	registerKeyword("piety", [this](const std::string& unused, std::istream& theStream) {
		const auto tempBlock = Character(theStream, charID);
		piety = tempBlock.getAccumulated();
	});
	registerKeyword("prestige", [this](const std::string& unused, std::istream& theStream) {
		const auto tempBlock = Character(theStream, charID);
		prestige = tempBlock.getAccumulated();
	});
	registerKeyword("accumulated", [this](const std::string& unused, std::istream& theStream) {
		accumulated = commonItems::singleDouble(theStream).getDouble();
	});
	registerKeyword("gold", [this](const std::string& unused, std::istream& theStream) {
		gold = commonItems::singleDouble(theStream).getDouble();
	});
	registerKeyword("court_data", [this](const std::string& unused, std::istream& theStream) {
		const auto tempBlock = Character(theStream, charID);
		employer = tempBlock.getEmployer();
		knight = tempBlock.isKnight();
		councilor = tempBlock.isCouncilor();
	});
	registerKeyword("employer", [this](const std::string& unused, std::istream& theStream) {
		employer = std::make_pair(commonItems::singleInt(theStream).getInt(), nullptr);
	});
	registerKeyword("knight", [this](const std::string& unused, std::istream& theStream) {
		knight = commonItems::singleString(theStream).getString() == "yes";
	});
	registerRegex("council_task|special_council_tasks", [this](const std::string& unused, std::istream& theStream) {
		councilor = true;
		commonItems::ignoreItem(unused, theStream);
	});
	registerKeyword("female", [this](const std::string& unused, std::istream& theStream) {
		female = commonItems::singleString(theStream).getString() == "yes";
	});
	registerKeyword("landed_data", [this](const std::string& unused, std::istream& theStream) {
		characterDomain = CharacterDomain(theStream);
	});
	registerKeyword("family_data", [this](const std::string& unused, std::istream& theStream) {
		const auto tempBlock = Character(theStream, charID);
		spouse = tempBlock.getSpouse();
	});
	registerKeyword("primary_spouse", [this](const std::string& unused, std::istream& theStream) {
		spouse = std::make_pair(commonItems::singleInt(theStream).getInt(), nullptr);
	});
	registerKeyword("claim", [this](const std::string& unused, std::istream& theStream) {
		const auto blobList = commonItems::blobList(theStream).getBlobs();
		for (const auto& blob: blobList)
		{
			auto blobStream = std::stringstream(blob);
			const auto titleID = Character(blobStream, charID).getTempTitle();
			if (titleID)
				claims.insert(std::pair(titleID, nullptr));
		}
	});
	registerKeyword("title", [this](const std::string& unused, std::istream& theStream) {
		tempTitle = commonItems::singleInt(theStream).getInt();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void CK3::Character::dropTitleFromDomain(int titleID)
{
	if (characterDomain)
	{
		std::vector<std::pair<int, std::shared_ptr<Title>>> replacementDomain;
		for (const auto& title: characterDomain->getDomain())
		{
			if (title.first != titleID)
				replacementDomain.emplace_back(title);
		}
		characterDomain->loadDomain(replacementDomain);
	}	
}
