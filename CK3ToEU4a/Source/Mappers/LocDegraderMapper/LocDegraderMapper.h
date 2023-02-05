#ifndef LOC_DEGRADER_MAPPER
#define LOC_DEGRADER_MAPPER
#include "Parser.h"

namespace mappers
{
class LocDegraderMapper: commonItems::parser
{
  public:
	LocDegraderMapper();
	explicit LocDegraderMapper(std::istream& theStream);

	[[nodiscard]] std::string degradeString(const std::string& inputString) const;

  private:
	void registerKeys();

	std::map<std::string, std::string> replacements;
};
} // namespace mappers

#endif // ISLAM_OVERRIDE_MAPPER