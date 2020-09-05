#ifndef CK3_CHARACTER_H
#define CK3_CHARACTER_H
#include "Date.h"
#include "Parser.h"
#include "CharacterDomain.h"

namespace CK3
{
class Title;
class House;
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

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getBirthDate() const { return birthDate; }
	[[nodiscard]] const auto& getCulture() const { return culture; }
	[[nodiscard]] const auto& getReligion() const { return religion; }
	[[nodiscard]] const auto& getHouse() const { return house; }
	[[nodiscard]] const auto& getSkills() const { return skills; }
	[[nodiscard]] auto getPiety() const { return piety; }
	[[nodiscard]] auto getPrestige() const { return prestige; }
	[[nodiscard]] auto getGold() const { return gold; }
	[[nodiscard]] const auto& getClaims() const { return claims; }
	[[nodiscard]] const auto& getEmployer() const { return employer; }
	[[nodiscard]] auto isKnight() const { return knight; }
	[[nodiscard]] auto isFemale() const { return female; }
	[[nodiscard]] const auto& getSpouse() const { return spouse; }
	
	[[nodiscard]] auto getID() const { return charID; }
	[[nodiscard]] auto getAccumulated() const { return accumulated; }
	[[nodiscard]] auto getTempTitle() const { return tempTitle; }

  private:
	void registerKeys();

	bool knight = false;
	bool female = false;
	int charID = 0;
	double piety = 0;
	double prestige = 0;
	double accumulated = 0; // temporary variable for recursive scrapes.
	int tempTitle = 0; // temporary variable for recursive scrapes.
	double gold = 0;
	std::string name;
	std::pair<int, std::string> culture;
	std::pair<int, std::string> religion;
	std::map<int, std::string> traits;
	std::pair<int, std::shared_ptr<Character>> employer;
	std::pair<int, std::shared_ptr<Character>> spouse;
	std::map<int, std::shared_ptr<Title>> claims;
	std::pair<int, std::shared_ptr<House>> house;
	
	Skills skills;
	CharacterDomain characterDomain;
	date birthDate = date("1.1.1");
	date deathDate = date("1.1.1");

};
} // namespace CK3

#endif // CK3_CHARACTER_H
