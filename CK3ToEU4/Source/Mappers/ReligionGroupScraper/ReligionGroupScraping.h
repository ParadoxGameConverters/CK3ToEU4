#ifndef RELIGION_GROUP_SCRAPING_H
#define RELIGION_GROUP_SCRAPING_H

#include <set>

#include "Parser.h"

namespace mappers
{
class ReligionGroupScraping: commonItems::parser
{
  public:
	explicit ReligionGroupScraping(std::istream& theStream);

	[[nodiscard]] const auto& getReligionNames() const { return religions; }

  private:
	void registerKeys();

	std::set<std::string> religions;
};
} // namespace mappers

#endif // RELIGION_GROUP_SCRAPING_H