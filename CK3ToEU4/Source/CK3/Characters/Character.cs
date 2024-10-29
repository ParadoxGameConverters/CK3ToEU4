
using CK3ToEU4.CK3.Characters;

namespace CK3
{

struct Skills
{
	int diplomacy = 0;
	int martial = 0;
	int stewardship = 0;
	int intrigue = 0;
	int learning = 0;
}

class Character
{
  public:
	Character(std::istream& theStream, long long characterID)
	{
		charID = characterID;
		
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}

	[[nodiscard]] auto isDead() const { return dead; }
	[[nodiscard]] auto isKnight() const { return knight; }
	[[nodiscard]] auto isFemale() const { return female; }
	[[nodiscard]] auto isCouncilor() const { return councilor; }
	[[nodiscard]] auto getID() const { return charID; }
	[[nodiscard]] auto getPiety() const { return piety; }
	[[nodiscard]] auto getPrestige() const { return prestige; }
	[[nodiscard]] auto getGold() const { return gold; }
	[[nodiscard]] auto isSpent() const { return spent; } // Used to mark characters (advisors) so they cannot be reused in different tag.
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getBirthDate() const { return birthDate; }

	[[nodiscard]] const auto& getCulture() const { return culture; }
	[[nodiscard]] const auto& getFaith() const { return faith; }
	[[nodiscard]] const auto& getEmployer() const { return employer; }
	[[nodiscard]] const auto& getSpouse() const { return spouse; }
	[[nodiscard]] const auto& getHouse() const { return house; }
	[[nodiscard]] const auto& getTraits() const { return traits; }
	[[nodiscard]] const auto& getClaims() const { return claims; }

	[[nodiscard]] const auto& getSkills() const { return skills; }
	[[nodiscard]] const auto& getCharacterDomain() const { return characterDomain; }
	[[nodiscard]] const auto& getCourtierNames() const { return courtierNames; }
	[[nodiscard]] const auto& getCouncilors() const { return councilors; }

	[[nodiscard]] auto getTempTitle() const { return tempTitle; }
	[[nodiscard]] auto getAccumulated() const { return accumulated; }

	void loadCulture(const std::pair<long long, std::shared_ptr<Culture>>& theCulture) { culture = theCulture; }
	void loadFaith(const std::pair<long long, std::shared_ptr<Faith>>& theFaith) { faith = theFaith; }
	void loadHouse(const std::pair<long long, std::shared_ptr<House>>& theHouse) { house = theHouse; }
	void loadRealmCapital(const std::pair<long long, std::shared_ptr<Title>>& theRealmCapital) { characterDomain->loadRealmCapital(theRealmCapital); }
	void loadDomain(const std::vector<std::pair<long long, std::shared_ptr<Title>>>& theDomain) { characterDomain->loadDomain(theDomain); }
	void loadClaims(const std::map<long long, std::shared_ptr<Title>>& theClaims) { claims = theClaims; }
	void loadEmployer(const std::pair<long long, std::shared_ptr<Character>>& theEmployer) { employer = theEmployer; }
	void loadSpouse(const std::pair<long long, std::shared_ptr<Character>>& theSpouse) { spouse = theSpouse; }
	void resetSpouse() { spouse.reset(); }
	void resetEmployer() { employer.reset(); }
	void loadTraits(const std::map<int, std::string>& theTraits) { traits = theTraits; }

	// processing
	void dropTitleFromDomain(long long titleID)
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
	void loadCourtierNames(const std::map<std::string, bool>& theNames) { courtierNames = theNames; }
	void loadCouncilors(const std::map<long long, std::shared_ptr<Character>>& newCouncilors) { councilors = newCouncilors; }

	// poking
	[[nodiscard]] bool hasTrait(const std::string& wantedTrait)
	{
		return std::any_of(traits.begin(), traits.end(), [wantedTrait](const std::pair<int, std::string>& trait) {
			return trait.second == wantedTrait;
		});
	}
	void setSpent() { spent = true; }

  private:
	void registerKeys()
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
}

	bool knight = false;
	bool female = false;
	bool councilor = false;
	long long charID = 0;
	double piety = 0;
	double prestige = 0;
	double gold = 0;
	std::string name;
	date birthDate = date("1.1.1");
	bool dead = false;
	bool spent = false;

	std::optional<std::pair<long long, std::shared_ptr<Culture>>> culture;
	std::optional<std::pair<long long, std::shared_ptr<Faith>>> faith;
	std::optional<std::pair<long long, std::shared_ptr<Character>>> employer;
	std::optional<std::pair<long long, std::shared_ptr<Character>>> spouse;
	std::pair<long long, std::shared_ptr<House>> house;
	std::map<int, std::string> traits;
	std::map<long long, std::shared_ptr<Title>> claims;

	Skills skills;
	std::optional<CharacterDomain> characterDomain;
	std::map<std::string, bool> courtierNames; // A simple list of people's names and genders. True=male.
	std::map<long long, std::shared_ptr<Character>> councilors;

	long long tempTitle = 0; // temporary variable for recursive scrapes.
	double accumulated = 0;	 // temporary variable for recursive scrapes.
}