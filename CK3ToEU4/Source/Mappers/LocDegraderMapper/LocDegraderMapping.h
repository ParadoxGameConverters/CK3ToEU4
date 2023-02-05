#ifndef LOC_DEGRADER_MAPPING
#define LOC_DEGRADER_MAPPING
#include "ConvenientParser.h"

namespace mappers
{
class LocDegraderMapping: commonItems::convenientParser
{
  public:
	LocDegraderMapping() = default;
	explicit LocDegraderMapping(std::istream& theStream);

	[[nodiscard]] const auto& getReplacementMapping() const { return replacementMapping; }

  private:
	void registerKeys();

	std::pair<std::string, std::string> replacementMapping;
};
} // namespace mappers

#endif // LOC_DEGRADER_MAPPING