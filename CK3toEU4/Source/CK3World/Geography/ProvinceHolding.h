#ifndef CK3_PROVINCEHOLDING_H
#define CK3_PROVINCEHOLDING_H
#include "Parser.h"
#include <set>

namespace CK3
{
class ProvinceHolding: commonItems::parser
{
  public:
	ProvinceHolding() = default;
	ProvinceHolding(std::istream& theStream);

	[[nodiscard]] const auto& getHoldingType() const { return holdingType; }
	[[nodiscard]] const auto& getBuildings() const { return buildings; }
	[[nodiscard]] const auto& getSpecialBuilding() const { return specialBuilding; }

  private:
	void registerKeys();

	std::string holdingType;
	std::set<std::string> buildings;
	std::string specialBuilding;
};
} // namespace CK3

#endif // CK3_PROVINCEHOLDING_H
