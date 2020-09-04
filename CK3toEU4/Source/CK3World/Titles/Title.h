#ifndef CK3_TITLE_H
#define CK3_TITLE_H
#include "Date.h"
#include "Parser.h"
#include <set>

namespace CK3
{
class Title: commonItems::parser
{
  public:
	Title(std::istream& theStream, int ID);
	[[nodiscard]] auto getID() const { return ID; }
	[[nodiscard]] const auto& getName() const { return name; }

  private:
	void registerKeys();

	int ID;											 // 11038
	std::string name;											 // c_ashmaka
	std::string displayName;								 // Ashmaka
	std::string adj;											 // Ashmakan
	date date;													 // Unclear. Ranges to 9999.1.1, probably is PDX alternative for "bool isCreated";
	std::pair<int, std::shared_ptr<Title>> dfLiege;	 // defacto liege title (d_kalyani)
	std::pair<int, std::shared_ptr<Title>> djLiege;	 // dejure liege title (d_rattapadi)
	std::map<int, std::shared_ptr<Title>> dfVassals; // defacto vassals, not in save, manually linked post-loading
	std::map<int, std::shared_ptr<Title>> djVassals; // dejure vassals (for all except baronies and titulars)
};
} // namespace CK3

#endif // CK3_TITLE_H
