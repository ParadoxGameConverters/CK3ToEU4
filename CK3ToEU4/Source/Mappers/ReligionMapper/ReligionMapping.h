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
	[[nodiscard]] const auto& getReligiousHead() const { return religiousHead; }

  private:
	void registerKeys();

	std::set<std::string> ck3Religions;
	std::string eu4Religion;
	std::optional<std::string> eu4School;
	std::optional<std::string> religiousHead;
};
} // namespace mappers

#endif // RELIGION_MAPPING_H