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
	Character(std::istream& theStream, int charID);

	[[nodiscard]] auto isKnight() const { return knight; }
	[[nodiscard]] auto isFemale() const { return female; }
	[[nodiscard]] auto getID() const { return charID; }
	[[nodiscard]] auto getPiety() const { return piety; }
	[[nodiscard]] auto getPrestige() const { return prestige; }
	[[nodiscard]] auto getGold() const { return gold; }
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

	[[nodiscard]] auto getTempTitle() const { return tempTitle; }
	[[nodiscard]] auto getAccumulated() const { return accumulated; }

	void loadCulture(const std::pair<int, std::shared_ptr<Culture>>& theCulture) { culture = theCulture; }
	void loadFaith(const std::pair<int, std::shared_ptr<Faith>>& theFaith) { faith = theFaith; }
	void loadHouse(const std::pair<int, std::shared_ptr<House>>& theHouse) { house = theHouse; }

  private:
	void registerKeys();

	bool knight = false;
	bool female = false;
	int charID = 0;
	double piety = 0;
	double prestige = 0;
	double gold = 0;
	std::string name;
	date birthDate = date("1.1.1");

	std::pair<int, std::shared_ptr<Culture>> culture;
	std::pair<int, std::shared_ptr<Faith>> faith;
	std::pair<int, std::shared_ptr<Character>> employer;
	std::pair<int, std::shared_ptr<Character>> spouse;
	std::pair<int, std::shared_ptr<House>> house;
	std::map<int, std::string> traits;
	std::map<int, std::shared_ptr<Title>> claims;

	Skills skills;
	CharacterDomain characterDomain;

	int tempTitle = 0;		// temporary variable for recursive scrapes.
	double accumulated = 0; // temporary variable for recursive scrapes.
};
} // namespace CK3

#endif // CK3_CHARACTER_H
