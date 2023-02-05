#ifndef CK3_HOUSENAMESCRAPING_H
#define CK3_HOUSENAMESCRAPING_H
#include "Parser.h"

namespace CK3
{
class HouseNameScraping: commonItems::parser
{
  public:
	HouseNameScraping() = default;
	HouseNameScraping(std::istream& theStream);
	[[nodiscard]] const auto& getPrefix() const { return prefix; }
	[[nodiscard]] const auto& getName() const { return name; }

  private:
	void registerKeys();

	std::string name;
	std::string prefix;
};
} // namespace CK3

#endif // CK3_HOUSENAMESCRAPING_H
