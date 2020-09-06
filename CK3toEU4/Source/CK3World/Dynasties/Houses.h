#ifndef CK3_HOUSES_H
#define CK3_HOUSES_H
#include "Parser.h"

namespace CK3
{
class House;
class Houses: commonItems::parser
{
  public:
	Houses() = default;
	explicit Houses(std::istream& theStream); // For testing
	
	[[nodiscard]] const auto& getHouses() const { return houses; }
	
  private:
	void registerKeys();

	std::map<int, std::shared_ptr<House>> houses;
};
} // namespace CK3

#endif // CK3_HOUSES_H
