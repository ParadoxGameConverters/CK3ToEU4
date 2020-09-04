#ifndef CK3_CHARACTER_H
#define CK3_CHARACTER_H
#include "Date.h"
#include "Parser.h"

namespace CK3
{
class Title;
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
	Character(std::istream& theStream, int chrID);

	[[nodiscard]] auto getID() const { return charID; }

  private:
	void registerKeys();

	int charID = 0;
	std::string name;
	std::pair<int, std::string> culture;
	std::pair<int, std::string> religion;
	std::map<int, std::string> traits;
	
	Skills skills;
	date birthDate = date("1.1.1");
	date deathDate = date("1.1.1");

};
} // namespace CK3

#endif // CK3_CHARACTER_H
