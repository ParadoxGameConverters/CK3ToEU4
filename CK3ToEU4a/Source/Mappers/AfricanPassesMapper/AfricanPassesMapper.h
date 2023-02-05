#ifndef AFRICAN_PASSES_MAPPER
#define AFRICAN_PASSES_MAPPER
#include "AfricanPassesMapping.h"
#include "Parser.h"

namespace mappers
{
class AfricanPassesMapper: commonItems::parser
{
  public:
	AfricanPassesMapper();
	explicit AfricanPassesMapper(std::istream& theStream);

	[[nodiscard]] const auto& getPasses() const { return passes; }

  private:
	void registerKeys();

	std::vector<AfricanPassesMapping> passes;
};
} // namespace mappers

#endif // AFRICAN_PASSES_MAPPER