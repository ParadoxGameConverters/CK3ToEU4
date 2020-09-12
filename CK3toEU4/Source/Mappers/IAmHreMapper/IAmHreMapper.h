#ifndef I_AM_HRE_MAPPER
#define I_AM_HRE_MAPPER

#include "Parser.h"

namespace mappers
{
class IAmHreMapper: commonItems::parser
{
  public:
	IAmHreMapper();
	explicit IAmHreMapper(std::istream& theStream);

	[[nodiscard]] const auto& getHRE() const { return hre; }

  private:
	void registerKeys();

	std::string hre;
};
} // namespace mappers

#endif // I_AM_HRE_MAPPER