#ifndef RELIGION_MAPPING_H
#define RELIGION_MAPPING_H

#include "Parser.h"
#include <set>

namespace mappers
{
class ReligionMapping: commonItems::parser
{
  public:
	explicit ReligionMapping(std::istream& theStream);

	[[nodiscard]] const auto& getCK3Religions() const { return ck3Religions; }
	[[nodiscard]] const auto& getEU4Religion() const { return eu4Religion; }
	[[nodiscard]] const auto& getEU4School() const { return eu4School; }

  private:
	void registerKeys();

	std::set<std::string> ck3Religions;
	std::string eu4Religion;
	std::optional<std::string> eu4School;
};
} // namespace mappers

#endif // RELIGION_MAPPING_H