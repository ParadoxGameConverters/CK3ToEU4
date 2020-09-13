#ifndef TRAIT_SCRAPING
#define TRAIT_SCRAPING
#include "Parser.h"

namespace mappers
{
class TraitScraping: commonItems::parser
{
  public:
	TraitScraping() = default;
	explicit TraitScraping(std::istream& theStream);

	[[nodiscard]] auto getIndex() const { return index; }

  private:
	void registerKeys();

	int index = 0;
};
} // namespace mappers

#endif // TRAIT_SCRAPING