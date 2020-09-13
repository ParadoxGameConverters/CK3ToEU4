#ifndef TRAIT_SCRAPER
#define TRAIT_SCRAPER
#include "Parser.h"

namespace mappers
{
class TraitScraper: commonItems::parser
{
  public:
	TraitScraper() = default;
	void loadTraits(std::istream& theStream);
	void loadTraits(const std::string& fileName);

	[[nodiscard]] const auto& getTraits() const { return traits; }

	[[nodiscard]] std::optional<std::string> getTraitForID(int ID) const;

  private:
	void registerKeys();

	int index = 0;
	std::map<int, std::string> traits;
};
} // namespace mappers

#endif // TRAIT_SCRAPER