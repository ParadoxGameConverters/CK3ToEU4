#ifndef CK3_BARONYHOLDINGS_H
#define CK3_BARONYHOLDINGS_H
#include "Parser.h"

namespace CK3
{
class BaronyHolding;
class BaronyHoldings: commonItems::parser
{
  public:
	BaronyHoldings() = default;
	explicit BaronyHoldings(std::istream& theStream);

	[[nodiscard]] const auto& getBaronyHoldings() const { return baronyHoldings; }

  private:
	void registerKeys();

	std::map<int, std::shared_ptr<BaronyHolding>> baronyHoldings;
};
} // namespace CK3

#endif // CK3_BARONYHOLDINGS_H
