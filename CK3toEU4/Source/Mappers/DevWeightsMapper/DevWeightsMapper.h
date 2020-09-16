#ifndef DEV_WEIGHTS_MAPPER
#define DEV_WEIGHTS_MAPPER

#include "Parser.h"

namespace mappers
{
class DevWeightsMapper: commonItems::parser
{
  public:
	DevWeightsMapper();
	explicit DevWeightsMapper(std::istream& theStream);

	[[nodiscard]] const auto& getDevFromBuilding() const { return devFromBuilding; }
	[[nodiscard]] const auto& getDevFromHolding() const { return devFromHolding; }
	[[nodiscard]] const auto& getDevFromDev() const { return devFromDev; }

  private:
	void registerKeys();

	double devFromBuilding = 0.1; // Default, unless overridden
	double devFromHolding = 0.3; // Default, unless overridden
	double devFromDev = 0.1;	// Default, unless overridden
};
} // namespace mappers

#endif // DEV_WEIGHTS_MAPPER