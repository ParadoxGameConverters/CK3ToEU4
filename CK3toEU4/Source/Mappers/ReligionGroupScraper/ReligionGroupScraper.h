#ifndef RELIGION_GROUP_SCRAPER_H
#define RELIGION_GROUP_SCRAPER_H

#include "Parser.h"
#include <map>
#include <optional>
#include <set>

namespace mappers
{
class ReligionGroupScraper: commonItems::parser
{
  public:
	ReligionGroupScraper();
	explicit ReligionGroupScraper(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> getReligionGroupForReligion(const std::string& religion) const;

  private:
	void registerKeys();
	std::map<std::string, std::set<std::string>> religionGroups; // religiongroupname, <religionnames>
};
} // namespace mappers

#endif // RELIGION_GROUP_SCRAPER_H
