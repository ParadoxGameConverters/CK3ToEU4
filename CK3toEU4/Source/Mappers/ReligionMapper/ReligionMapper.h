#ifndef RELIGION_MAPPER_H
#define RELIGION_MAPPER_H

#include "Parser.h"
#include <map>
#include <optional>
#include <string>

namespace CK3
{
class Faiths;
class Faith;
}

namespace mappers
{
class ReligionMapper: commonItems::parser
{
  public:
	ReligionMapper();
	explicit ReligionMapper(std::istream& theStream);	
	void importCK3Faiths(const CK3::Faiths& faiths);
	
	[[nodiscard]] std::optional<std::string> getEU4ReligionForCK3Religion(const std::string& ck3Religion) const;

  private:
	void registerKeys();
	void importCK3Faith(const CK3::Faith& faith);

	std::map<std::string, std::string> CK3toEU4ReligionMap;
};
} // namespace mappers

#endif // RELIGION_MAPPER_H
