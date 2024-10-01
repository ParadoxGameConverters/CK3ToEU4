#ifndef CK3_HOUSENAMESCRAPER_H
#define CK3_HOUSENAMESCRAPER_H
#include "HouseNameScraping.h"
#include "Parser.h"

namespace CK3
{
class HouseNameScraper: commonItems::parser
{
  public:
	HouseNameScraper() = default;
	void loadHouseDetails(std::istream& theStream);
	void loadHouseDetails(const std::string& filename);

	[[nodiscard]] std::optional<std::string> getNameForKey(const std::string& houseKey) const;
	[[nodiscard]] std::optional<std::string> getPrefixForKey(const std::string& houseKey) const;
	[[nodiscard]] const auto& getHouseNames() const { return houseNames; }

  private:
	void registerKeys();

	std::map<std::string, HouseNameScraping> houseNames;
};
} // namespace CK3

#endif // CK3_HOUSENAMESCRAPER_H
