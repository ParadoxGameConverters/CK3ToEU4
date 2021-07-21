#ifndef VASSAL_SPLITOFF_MAPPER
#define VASSAL_SPLITOFF_MAPPER

#include "ConvenientParser.h"

namespace mappers
{
class VassalSplitoffMapper: commonItems::convenientParser
{
  public:
	VassalSplitoffMapper();
	explicit VassalSplitoffMapper(std::istream& theStream);

	[[nodiscard]] double getFactor() const;

  private:
	void registerKeys();

	int sensitivity = 100;
};
} // namespace mappers

#endif // VASSAL_SPLITOFF_MAPPER