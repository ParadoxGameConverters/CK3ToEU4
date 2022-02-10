#ifndef HERITAGE_MAPPER
#define HERITAGE_MAPPER
#include "Parser.h"

namespace mappers
{
class HeritageMapper: commonItems::parser
{
  public:
	HeritageMapper();
	explicit HeritageMapper(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> getCultureGroupForHeritage(const std::string& heritage) const;

  private:
	void registerKeys();

	std::map<std::string, std::string> heritagesToCultures;
};
} // namespace mappers

#endif // HERITAGE_MAPPER