#ifndef SHATTER_EMPIRES_MAPPER
#define SHATTER_EMPIRES_MAPPER

#include "Parser.h"
#include <set>

namespace mappers
{
class ShatterEmpiresMapper: commonItems::parser
{
  public:
	ShatterEmpiresMapper();
	explicit ShatterEmpiresMapper(std::istream& theStream);

	[[nodiscard]] const auto& getEmpires() const { return empires; }
	[[nodiscard]] bool isEmpireShatterable(const std::string& empire) const { return empires.count(empire); }

  private:
	void registerKeys();

	std::set<std::string> empires;
};
} // namespace mappers

#endif // SHATTER_EMPIRES_MAPPER