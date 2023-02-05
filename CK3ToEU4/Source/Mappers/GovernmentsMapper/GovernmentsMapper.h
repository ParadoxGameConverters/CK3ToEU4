#ifndef GOVERNMENTS_MAPPER
#define GOVERNMENTS_MAPPER

#include "GovernmentsMapping.h"
#include "Parser.h"

namespace mappers
{
class GovernmentsMapper: commonItems::parser
{
  public:
	GovernmentsMapper();
	explicit GovernmentsMapper(std::istream& theStream);

	[[nodiscard]] std::optional<std::pair<std::string, std::string>> matchGovernment(const std::string& ck3Government, const std::string& ck3Title) const;

  private:
	void registerKeys();
	std::vector<GovernmentsMapping> govMappings;
};
} // namespace mappers

#endif // GOVERNMENTS_MAPPER