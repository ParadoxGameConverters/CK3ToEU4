#ifndef CK3_HOUSE_H
#define CK3_HOUSE_H
#include "Parser.h"

namespace CK3
{
class Dynasty;
class Character;
class House: commonItems::parser
{
  public:
	House() = default;
	House(std::istream& theStream, long long housID);
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getLocalizedName() const { return localizedName; }
	[[nodiscard]] const auto& getPrefix() const { return prefix; }
	[[nodiscard]] const auto& getDynasty() const { return dynasty; }
	[[nodiscard]] const auto& getID() const { return houseID; }
	[[nodiscard]] const auto& getHouseHead() const { return houseHead; }

	void loadDynasty(const std::pair<long long, std::shared_ptr<Dynasty>>& theDynasty) { dynasty = theDynasty; }
	void loadHouseHead(const std::pair<long long, std::shared_ptr<Character>>& theHead) { houseHead = theHead; }
	void resetHouseHead() { houseHead.reset(); }

  private:
	void registerKeys();

	long long houseID = 0;
	std::string name;
	std::string localizedName;
	std::pair<long long, std::shared_ptr<Dynasty>> dynasty;
	std::string prefix;
	std::optional<std::pair<long long, std::shared_ptr<Character>>> houseHead; // houses can have missing heads or dead people...
};
} // namespace CK3

#endif // CK3_HOUSE_H
