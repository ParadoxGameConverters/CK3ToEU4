#ifndef CK3_COUNTYDETAIL_H
#define CK3_COUNTYDETAIL_H
#include "Parser.h"

namespace CK3
{
class Faith;
class CountyDetail: commonItems::parser
{
  public:
	CountyDetail() = default;
	CountyDetail(std::istream& theStream);

	[[nodiscard]] auto getDevelopment() const { return development; }
	[[nodiscard]] const auto& getCulture() const { return culture; }
	[[nodiscard]] const auto& getFaith() const { return faith; }

  private:
	void registerKeys();

	int development = 0;
	std::pair<int, std::string> culture;
	std::pair<int, std::shared_ptr<Faith>> faith;
};
} // namespace CK3

#endif // CK3_COUNTYDETAIL_H
