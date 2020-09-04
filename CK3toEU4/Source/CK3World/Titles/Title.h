#ifndef CK3_TITLE_H
#define CK3_TITLE_H
#include "Date.h"
#include "Parser.h"
#include <set>

namespace CK3
{
class Character;
class CoatOfArms;
class Title: commonItems::parser
{
  public:
	Title(std::istream& theStream, int ID);
	[[nodiscard]] auto getID() const { return ID; }
	[[nodiscard]] auto isTheocraticLease() const { return theocraticLease; }
	[[nodiscard]] auto isCountyCapitalBarony() const { return cCapitalBarony; }
	[[nodiscard]] auto isDuchyCapitalBarony() const { return dCapitalBarony; }
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
	[[nodiscard]] const auto& getLaws() const { return laws; }
	[[nodiscard]] const auto& getHolder() const { return holder; }
	[[nodiscard]] const auto& getCoA() const { return coa; }

  private:
	void registerKeys();

	int ID = 0;																		// 11038
	std::pair<int, std::shared_ptr<Title>> capital;						// capital title is a COUNTY, even for county itself and baronies beneath it!
	std::string name;																// c_ashmaka
	std::string displayName;													// Ashmaka
	std::string adjective;														// Ashmakan
	std::string article;															// "the ". Not always present.
	std::string historyGovernment;											// Unclear why this is history. Maybe further governments override it.
	date creationDate;															// Unclear. Ranges to 9999.1.1, probably is PDX alternative for "bool isCreated";
	std::pair<int, std::shared_ptr<CoatOfArms>> coa;					// This is dejure flag but not defacto.
	std::pair<int, std::shared_ptr<Title>> dfLiege;						// defacto liege title (d_kalyani)
	std::pair<int, std::shared_ptr<Title>> djLiege;						// dejure liege title (d_rattapadi)
	std::pair<int, std::shared_ptr<Character>> holder;					// Holding character
	std::map<int, std::shared_ptr<Title>> dfVassals;					// defacto vassals, not in save, manually linked post-loading
	std::map<int, std::shared_ptr<Title>> djVassals;					// dejure vassals (for all except baronies and titulars)
	std::vector<std::pair<int, std::shared_ptr<Character>>> heirs; // Order of heirs is unclear so we're keeping them ordered and using fist if able.
	std::map<int, std::shared_ptr<Character>> claims;					// Incredibly useful.
	bool theocraticLease = false;												// Does this apply to non-baronies? Maybe? Who owns it then, dejure liege?
	std::set<std::string> laws;
	bool cCapitalBarony = false; 
	bool dCapitalBarony = false;
};
} // namespace CK3

#endif // CK3_TITLE_H
