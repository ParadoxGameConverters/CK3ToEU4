#ifndef CK3_RELIGION_SCRAPING_H
#define CK3_RELIGION_SCRAPING_H
#include "Parser.h"
#include <set>

namespace mappers
{
class CK3ReligionScraping: commonItems::parser
{
  public:
	explicit CK3ReligionScraping(std::istream& theStream);

	[[nodiscard]] const auto& getFaithNames() const { return faiths; }

  private:
	parser faithParser;

	void registerKeys();

	std::set<std::string> faiths;
};
} // namespace mappers

#endif // CK3_RELIGION_SCRAPING_H