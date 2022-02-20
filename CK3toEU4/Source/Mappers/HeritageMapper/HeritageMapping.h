#ifndef HERITAGE_MAPPING
#define HERITAGE_MAPPING
#include "ConvenientParser.h"
#include <set>

namespace mappers
{
class HeritageMapping: commonItems::convenientParser
{
  public:
	HeritageMapping() = default;
	explicit HeritageMapping(std::istream& theStream);

	[[nodiscard]] const auto& getEU4CultureGroup() const { return eu4CultureGroup; }
	[[nodiscard]] const auto& getCK3Heritages() const { return ck3Heritages; }

  private:
	void registerKeys();

	std::string eu4CultureGroup;
	std::set<std::string> ck3Heritages;
};
} // namespace mappers

#endif // HERITAGE_MAPPING