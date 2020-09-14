#ifndef EU4_COUNTRY_DETAILS_H
#define EU4_COUNTRY_DETAILS_H

#include "Color.h"
#include "Date.h"
#include "Parser.h"
#include <set>

namespace EU4
{

typedef struct Character
{
	bool isSet = false;
	bool regency = false;
	bool discount = false;
	bool female = false;
	bool regent = false;
	int adm = 0;
	int dip = 0;
	int mil = 0;
	int claim = 0;
	int id = 0; // advisers
	int location = 0;
	int skill = 0;
	std::string name;
	std::string monarchName; // for heirs
	std::string dynasty;
	std::string religion;
	std::string culture;
	std::string originCountry;
	std::set<std::string> personalities;
	std::string type;
	date birthDate;
	date deathDate = date("1.1.1");
	date appearDate = date("1.1.1");
} Character;

class CountryDetails: commonItems::parser
{
  public:
	CountryDetails() = default;
	explicit CountryDetails(const std::string& filePath);
	explicit CountryDetails(std::istream& theStream);

	void parseHistory(const std::string& filePath);
	void parseHistory(std::istream& theStream);

	// These values are open to ease management.
	// This is a storage container for EU4::Country.
	// Some of these values are loaded from files, others are adjusted on the fly.
	bool inHRE = false;
	bool holyRomanEmperor = false;
	bool celestialEmperor = false;
	bool fixedCapital = false;
	bool elector = false;
	bool randomChance = false;
	bool all_your_core_are_belong_to_us = false;
	bool rightToBEARArms = false;
	bool loan = false;
	bool excommunicated = false;
	bool hasDynastyName = false; // Country name comes from dynasty rather than Title. (Ottomans, Seljuks, Abbasids, etc.)
	bool isSunsetCountry = false;
	int addTreasury = 0;
	int addPrestige = 0;
	int dynastyID = 0;
	int capital = 0;
	int governmentRank = 1;
	int mercantilism = 0;
	int historicalScore = 0;
	int addedAdminTech = 0;
	int addedDipTech = 0;
	int addedMilTech = 0;
	double armyProfessionalism = 0;
	double piety = 0;
	std::optional<commonItems::Color> color;
	std::optional<commonItems::Color> revolutionaryColor;

	std::string primaryCulture = "noculture";
	std::string majorityReligion = "noreligion";
	std::string religion = "noreligion";
	std::string graphicalCulture = "westerngfx";
	std::string government = "monarchy";
	std::string technologyGroup = "western";
	std::string unitType;
	std::string religiousSchool;
	std::string nationalFocus;
	std::string secondaryReligion;
	std::string preferredReligion;
	std::string colonialParent;
	std::string specialUnitCulture;
	std::set<std::string> acceptedCultures;
	std::set<std::string> reforms;
	std::set<std::string> cults;
	std::set<std::string> historicalRivals;
	std::set<std::string> historicalFriends;
	std::set<std::string> harmonizedReligions;
	std::vector<std::string> historicalIdeaGroups;
	std::set<std::string> historicalUnits;
	std::set<std::string> leaderNames;
	std::set<std::string> shipNames;
	std::set<std::string> armyNames;
	std::set<std::string> fleetNames;
	std::map<std::string, std::pair<int, int>> monarchNames;	 // name (without regnal), pair(regnal, chance)	
	std::vector<std::pair<date, std::string>> historyLessons; // this is used to store history entries for countries we're only transcribing.

	Character monarch;
	Character queen;
	Character heir;
	std::vector<Character> advisers;

  private:
	void registerKeys();
	void registerHistoryKeys();
};
} // namespace EU4

#endif // EU4_COUNTRY_DETAILS_H
