#ifndef START_DATE_MAPPER
#define START_DATE_MAPPER

#include "Parser.h"

namespace mappers
{
class StartDateMapper: commonItems::parser
{
  public:
	StartDateMapper();
	explicit StartDateMapper(std::istream& theStream);

	[[nodiscard]] const auto& getStartDate() const { return startDate; }

  private:
	void registerKeys();
	void validateDate();
	void fallbackToDefault();

	date startDate;
	int day = 0;
	int month = 0;
	int year = 0;
};
} // namespace mappers

#endif // START_DATE_MAPPER