#ifndef CK3_DYNASTIES_H
#define CK3_DYNASTIES_H
#include "Houses.h"
#include "Parser.h"

namespace CK3
{
class Dynasty;
class CoatsOfArms;
class Dynasties: commonItems::parser
{
  public:
	Dynasties() = default;
	explicit Dynasties(std::istream& theStream);

	[[nodiscard]] const auto& getDynasties() const { return dynasties; }
	[[nodiscard]] auto getHouses() { return std::move(houses); }

	void linkCoats(const CoatsOfArms& coats);

  private:
	void registerKeys();

	Houses houses;
	std::map<int, std::shared_ptr<Dynasty>> dynasties;
};
} // namespace CK3

#endif // CK3_DYNASTIES_H
