#ifndef CK3_RELIGION_SCRAPER_H
#define CK3_RELIGION_SCRAPER_H
#include "BulkParser.h"
#include <map>
#include <set>

namespace mappers
{
class CK3ReligionScraper: commonItems::bulkParser
{
  public:
	CK3ReligionScraper() = default;
	void scrapeCK3Religions(const commonItems::ModFilesystem& modFS);

	[[nodiscard]] std::set<std::string> getSimilarFaiths(const std::string& faith) const;

  private:
	void registerKeys();

	std::map<std::string, std::set<std::string>> religionFaiths; // religionname, <faithnames>
};
} // namespace mappers

#endif // CK3_RELIGION_SCRAPER_H
