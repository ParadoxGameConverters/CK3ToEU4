#ifndef CK3_CULTURES_H
#define CK3_CULTURES_H
#include "Parser.h"

namespace CK3
{
class Culture;
class Cultures: commonItems::parser
{
  public:
	Cultures() = default;
	explicit Cultures(std::istream& theStream);

	[[nodiscard]] const auto& getCultures() const { return cultures; }

  private:
	void registerKeys();

	std::map<long long, std::shared_ptr<Culture>> cultures;
};
} // namespace CK3

#endif // CK3_CULTURES_H
