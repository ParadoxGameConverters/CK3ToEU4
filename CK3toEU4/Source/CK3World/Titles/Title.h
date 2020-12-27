#ifndef CK3_TITLE_H
#define CK3_TITLE_H
#include "Color.h"
#include "Date.h"
#include "Parser.h"
#include <set>

namespace mappers
{
class DevWeightsMapper;
}
namespace EU4
{
class Country;
}

namespace CK3
{
class Character;
class CoatOfArms;
class LandedTitles;

enum class LEVEL
{
	BARONY,
	COUNTY,
	DUCHY,
	KINGDOM,
	EMPIRE
};

static std::map<int, LEVEL> IntToLevel{{0, LEVEL::BARONY}, {1, LEVEL::COUNTY}, {2, LEVEL::DUCHY}, {3, LEVEL::KINGDOM}, {4, LEVEL::EMPIRE}};
static std::map<LEVEL, int> LevelToInt{{LEVEL::BARONY, 0}, {LEVEL::COUNTY, 1}, {LEVEL::DUCHY, 2}, {LEVEL::KINGDOM, 3}, {LEVEL::EMPIRE, 4}};

class Title: commonItems::parser
{
  public:
	Title(std::istream& theStream, long long theID);
	[[nodiscard]] auto getID() const { return ID; }
	[[nodiscard]] auto isTheocraticLease() const { return theocraticLease; }
	[[nodiscard]] auto isCountyCapitalBarony() const { return cCapitalBarony; }
	[[nodiscard]] auto isDuchyCapitalBarony() const { return dCapitalBarony; }
	[[nodiscard]] auto isHREEmperor() const { return HREEmperor; }
	[[nodiscard]] auto isInHRE() const { return inHRE; }
	[[nodiscard]] auto isThePope() const { return thePope; }
	[[nodiscard]] auto isElectorate() const { return electorate; }
	[[nodiscard]] auto isHolderCapital() const { return holderCapital; }
	[[nodiscard]] auto isHRECapital() const { return HRECapital; }
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getDisplayName() const { return displayName; }
	[[nodiscard]] const auto& getAdjective() const { return adjective; }
	[[nodiscard]] const auto& getCreationDate() const { return creationDate; }
	[[nodiscard]] const auto& getHistoryGovernment() const { return historyGovernment; }
	[[nodiscard]] const auto& getCapital() const { return capital; }
	[[nodiscard]] const auto& getDFLiege() const { return dfLiege; }
	[[nodiscard]] const auto& getDJLiege() const { return djLiege; }
	[[nodiscard]] const auto& getDFVassals() const { return dfVassals; }
	[[nodiscard]] const auto& getDJVassals() const { return djVassals; }
	[[nodiscard]] const auto& getHeirs() const { return heirs; }
	[[nodiscard]] const auto& getClaimants() const { return claimants; }
	[[nodiscard]] const auto& getElectors() const { return electors; }
	[[nodiscard]] const auto& getLaws() const { return laws; }
	[[nodiscard]] const auto& getHolder() const { return holder; }
	[[nodiscard]] const auto& getCoA() const { return coa; }
	[[nodiscard]] const auto& getClay() const { return clay; }
	[[nodiscard]] const auto& getOwnedDFCounties() const { return ownedDFCounties; }
	[[nodiscard]] const auto& getOwnedDJCounties() const { return ownedDJCounties; }
	[[nodiscard]] const auto& getHoldingTitle() const { return holdingTitle; }
	[[nodiscard]] const auto& getGeneratedLiege() const { return generatedLiege; }
	[[nodiscard]] const auto& getGeneratedVassals() const { return generatedVassals; }
	[[nodiscard]] const auto& getEU4Tag() const { return tagCountry; }
	[[nodiscard]] const auto& getPreviousHolders() const { return previousHolders; }

	[[nodiscard]] std::optional<commonItems::Color> getColor() const;
	[[nodiscard]] bool isLandless() const;
	[[nodiscard]] bool areDFLiegeAndDJLiegeSet() const;
	[[nodiscard]] bool areDFLiegeAndDJLiegeSame() const;
	[[nodiscard]] bool isHolderSet() const { return holder != std::nullopt; }
	[[nodiscard]] bool isHolderLinked() const { return holder && holder->second; }
	[[nodiscard]] bool doesHolderHaveCharacterDomain() const;
	[[nodiscard]] std::set<std::string> getTitleNamesFromHolderDomain() const;
	[[nodiscard]] LEVEL getLevel() const;

	// linkage
	void loadCoat(const std::pair<long long, std::shared_ptr<CoatOfArms>>& coat) { coa = coat; }
	void loadDFLiege(const std::pair<long long, std::shared_ptr<Title>>& DFLiege) { dfLiege = DFLiege; }
	void loadDJLiege(const std::pair<long long, std::shared_ptr<Title>>& DJLiege) { djLiege = DJLiege; }
	void loadDFVassals(const std::map<long long, std::shared_ptr<Title>>& DFVassals) { dfVassals = DFVassals; }
	void addDFVassals(const std::map<long long, std::shared_ptr<Title>>& DFVassals) { dfVassals.insert(DFVassals.begin(), DFVassals.end()); }
	void loadDJVassals(const std::map<long long, std::shared_ptr<Title>>& DJVassals) { djVassals = DJVassals; }
	void loadHolder(const std::pair<long long, std::shared_ptr<Character>>& theHolder) { holder = theHolder; }
	void loadHeirs(const std::vector<std::pair<long long, std::shared_ptr<Character>>>& theHeirs) { heirs = theHeirs; }
	void loadPreviousHolders(const std::vector<std::pair<long long, std::shared_ptr<Character>>>& theHolders) { previousHolders = theHolders; }
	void loadClaimants(const std::map<long long, std::shared_ptr<Character>>& theClaimants) { claimants = theClaimants; }
	void loadElectors(const std::map<long long, std::shared_ptr<Character>>& theElectors) { electors = theElectors; }
	void loadClay(const std::shared_ptr<LandedTitles>& theClay) { clay = theClay; }
	void loadOwnedDFCounties(const std::map<std::string, std::shared_ptr<Title>>& theOwnedCounties) { ownedDFCounties = theOwnedCounties; }
	void loadOwnedDJCounties(const std::map<std::string, std::shared_ptr<Title>>& theOwnedCounties) { ownedDJCounties = theOwnedCounties; }

	// processing
	void setDynamicLevel(LEVEL theLevel) { dynamicLevel = theLevel; }
	void cleanUpDisplayName();
	[[nodiscard]] int flagDeJureHREProvinces();
	void brickTitle();
	void grantIndependence();
	void resetDFLiege() { dfLiege.reset(); }
	void resetDJLiege() { djLiege.reset(); }
	void setHREEmperor() { HREEmperor = true; }
	void setInHRE() { inHRE = true; }
	void dropTitleFromDFVassals(long long titleID);
	void setThePope() { thePope = true; }
	void congregateDFCounties();
	void congregateDJCounties();
	void loadGeneratedLiege(const std::pair<std::string, std::shared_ptr<Title>>& liege) { generatedLiege = liege; }
	void addGeneratedVassal(const std::pair<std::string, std::shared_ptr<Title>>& theVassal) { generatedVassals.insert(theVassal); }
	void loadHoldingTitle(const std::pair<std::string, std::shared_ptr<Title>>& theTitle) { holdingTitle = theTitle; }
	void setElectorate() { electorate = true; }
	void relinkDeFactoVassals();

	// conversion
	void loadEU4Tag(const std::pair<std::string, std::shared_ptr<EU4::Country>>& theCountry) { tagCountry = theCountry; }
	[[nodiscard]] double getBuildingWeight(const mappers::DevWeightsMapper& devWeightsMapper) const;
	void setHolderCapital() { holderCapital = true; }
	void setHRECapital() { HRECapital = true; }
	void clearGeneratedVassals() { generatedVassals.clear(); }

	[[nodiscard]] std::map<std::string, std::shared_ptr<Title>> coalesceDFCounties() const;
	[[nodiscard]] std::map<std::string, std::shared_ptr<Title>> coalesceDJCounties() const;

  private:
	void registerKeys();

	long long ID = 0;																			// 11038
	std::pair<long long, std::shared_ptr<Title>> capital;							// capital title is a COUNTY, even for county itself and baronies beneath it!
	std::string name;																			// c_ashmaka
	std::string displayName;																// Ashmaka
	std::string adjective;																	// Ashmakan
	std::string article;																		// "the ". Not always present.
	std::string historyGovernment;														// Unclear why this is history. Maybe further governments override it.
	date creationDate;																		// Unclear. Ranges to 9999.1.1, probably is PDX alternative for "bool isCreated";
	std::optional<std::pair<long long, std::shared_ptr<CoatOfArms>>> coa;	// This is dejure flag but not defacto.
	std::optional<std::pair<long long, std::shared_ptr<Title>>> dfLiege;		// defacto liege title (d_kalyani)
	std::optional<std::pair<long long, std::shared_ptr<Title>>> djLiege;		// dejure liege title (d_rattapadi)
	std::optional<std::pair<long long, std::shared_ptr<Character>>> holder; // Holding character
	std::map<long long, std::shared_ptr<Title>> dfVassals;						// defacto vassals, not in save, manually linked post-loading
	std::map<long long, std::shared_ptr<Title>> djVassals;						// dejure vassals (for all except baronies and titulars)
	std::vector<std::pair<long long, std::shared_ptr<Character>>> heirs;		// Order of heirs is unclear so we're keeping them ordered and using first if able.
	std::map<long long, std::shared_ptr<Character>> claimants;					// People holding a claim to this title. Incredibly useful.
	std::map<long long, std::shared_ptr<Character>> electors;					// People involved in elections regardless of election type law.
	bool theocraticLease = false;															// Does this apply to non-baronies? Maybe? Who owns it then, dejure liege?
	std::set<std::string> laws;
	bool cCapitalBarony = false;
	bool dCapitalBarony = false;
	std::shared_ptr<LandedTitles> clay; // Middleware towards geographical data, essential for b_&c_, potentially obsolete for others.
	bool HREEmperor = false;
	bool inHRE = false;
	bool thePope = false;
	std::map<std::string, std::shared_ptr<Title>> ownedDFCounties; // used to map higher-lvl titles directly to clay. Includes self! Every c_+ title has this.
	std::map<std::string, std::shared_ptr<Title>> ownedDJCounties; // ditto
	std::optional<std::pair<std::string, std::shared_ptr<Title>>> generatedLiege; // Liege we set manually while splitting vassals.
	std::map<std::string, std::shared_ptr<Title>> generatedVassals;					// Vassals we split off deliberately.
	std::pair<std::string, std::shared_ptr<Title>> holdingTitle;						// topmost owner title (e_francia or similar), only c_s have this.
	bool electorate = false;
	std::optional<std::pair<std::string, std::shared_ptr<EU4::Country>>> tagCountry;
	bool landless = false;
	std::optional<commonItems::Color> color;
	std::vector<std::pair<long long, std::shared_ptr<Character>>> previousHolders;
	bool holderCapital = false;
	bool HRECapital = false;
	std::optional<LEVEL> dynamicLevel; // Maybe assigned through dynamic ranks, otherwise getLevel will try to guesstimate.
};
} // namespace CK3

#endif // CK3_TITLE_H
