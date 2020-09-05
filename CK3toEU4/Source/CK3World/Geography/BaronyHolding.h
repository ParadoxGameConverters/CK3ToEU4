#ifndef CK3_BARONYHOLDING_H
#define CK3_BARONYHOLDING_H
#include "Parser.h"
#include <set>

namespace CK3
{
class BaronyHolding: commonItems::parser
{
  public:
	BaronyHolding() = default;
	BaronyHolding(std::istream& theStream);

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

#endif // CK3_BARONYHOLDING_H
