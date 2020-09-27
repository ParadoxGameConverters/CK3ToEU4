#ifndef CK3_HOUSES_H
#define CK3_HOUSES_H
#include "Parser.h"

namespace CK3
{
class House;
class Dynasties;
class Characters;
class Houses: commonItems::parser
{
  public:
	Houses() = default;
	explicit Houses(std::istream& theStream);

	[[nodiscard]] const auto& getHouses() const { return houses; }

	void linkDynasties(const Dynasties& dynasties);
	void linkCharacters(const Characters& characters);

  private:
	void registerKeys();

	std::map<long long, std::shared_ptr<House>> houses;
};
} // namespace CK3

#endif // CK3_HOUSES_H
