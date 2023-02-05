#ifndef ISLAM_OVERRIDE_MAPPING
#define ISLAM_OVERRIDE_MAPPING
#include "ConvenientParser.h"

namespace mappers
{
class IslamOverrideMapping: commonItems::convenientParser
{
  public:
	IslamOverrideMapping() = default;
	explicit IslamOverrideMapping(std::istream& theStream);

	[[nodiscard]] const auto& getReplacementMapping() const { return replacementMapping; }

  private:
	void registerKeys();

	std::pair<int, std::string> replacementMapping;
};
} // namespace mappers

#endif // ISLAM_OVERRIDE_MAPPING