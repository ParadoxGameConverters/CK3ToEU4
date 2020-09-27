#ifndef CK3_CHARACTER_H
#define CK3_CHARACTER_H
#include "CharacterDomain.h"
#include "Date.h"
#include "Parser.h"

namespace CK3
{
class Title;
class House;
class Faith;
class Culture;
typedef struct Skills
{
	int diplomacy = 0;
	int martial = 0;
	int stewardship = 0;
	int intrigue = 0;
	int learning = 0;
} Skills;

class Character: commonItems::parser
{
  public:
	Character(std::istream& theStream, long long characterID);

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
	[[nodiscard]] const auto& getDomain() const { return characterDomain; }
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
	void loadTraits(const std::map<int, std::string>& theTraits) { traits = theTraits; }

	// processing
	void dropTitleFromDomain(long long titleID);
	void loadCourtierNames(const std::map<std::string, bool>& theNames) { courtierNames = theNames; }
	void loadCouncilors(const std::map<long long, std::shared_ptr<Character>>& newCouncilors) { councilors = newCouncilors; }

	// poking
	[[nodiscard]] bool hasTrait(const std::string& wantedTrait) const;
	void setSpent() { spent = true; }

  private:
	void registerKeys();

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
	double accumulated = 0; // temporary variable for recursive scrapes.
};
} // namespace CK3

#endif // CK3_CHARACTER_H
