#ifndef ISLAM_OVERRIDE_MAPPER
#define ISLAM_OVERRIDE_MAPPER
#include "Parser.h"

namespace mappers
{
class IslamOverrideMapper: commonItems::parser
{
  public:
	IslamOverrideMapper();
	explicit IslamOverrideMapper(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> getReplacementReligionForProvince(int provinceID) const;

  private:
	void registerKeys();

	std::map<int, std::string> replacements;
};
} // namespace mappers

#endif // ISLAM_OVERRIDE_MAPPER