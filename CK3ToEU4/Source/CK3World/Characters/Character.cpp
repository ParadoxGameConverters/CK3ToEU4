#include "Character.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Character::Character(std::istream& theStream, long long characterID): charID(characterID)
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
	registerKeyword("dead_data", [this](const std::string& unused, std::istream& theStream) {
		commonItems::ignoreItem(unused, theStream);
		dead = true;
	});
	registerKeyword("culture", [this](const std::string& unused, std::istream& theStream) {
		culture = std::pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
		if (culture->first == 4294967295)
			culture.reset();
	});
	registerKeyword("faith", [this](const std::string& unused, std::istream& theStream) {
		faith = std::pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
		if (faith->first == 4294967295)
			faith.reset();
	});
	registerKeyword("dynasty_house", [this](const std::string& unused, std::istream& theStream) {
		house = std::pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
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
		suzerain = tempBlock.getSuzerain();
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
	registerKeyword("obedience_target", [this](const std::string& unused, std::istream& theStream) {
		suzerain = std::make_pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
	});
	registerKeyword("gold", [this](const std::string& unused, std::istream& theStream) {
		goldParser.parseStream(theStream);
	});
	registerKeyword("court_data", [this](const std::string& unused, std::istream& theStream) {
		const auto tempBlock = Character(theStream, charID);
		employer = tempBlock.getEmployer();
		knight = tempBlock.isKnight();
		councilor = tempBlock.isCouncilor();
	});
	registerKeyword("employer", [this](const std::string& unused, std::istream& theStream) {
		employer = std::make_pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
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
		spouse = std::make_pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
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
		tempTitle = commonItems::singleLlong(theStream).getLlong();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	goldParser.registerKeyword("value", [this](const std::string& unused, std::istream& theStream) {
		gold = commonItems::singleDouble(theStream).getDouble();
	});

	goldParser.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void CK3::Character::dropTitleFromDomain(long long titleID)
{
	if (characterDomain)
	{
		std::vector<std::pair<long long, std::shared_ptr<Title>>> replacementDomain;
		for (const auto& title: characterDomain->getDomain())
		{
			if (title.first != titleID)
				replacementDomain.emplace_back(title);
		}
		characterDomain->loadDomain(replacementDomain);
	}
}

bool CK3::Character::hasTrait(const std::string& wantedTrait) const
{
	return std::any_of(traits.begin(), traits.end(), [wantedTrait](const std::pair<int, std::string>& trait) {
		return trait.second == wantedTrait;
	});
}
