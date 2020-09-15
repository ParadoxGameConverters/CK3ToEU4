#ifndef RELIGION_MAPPER_H
#define RELIGION_MAPPER_H

#include "Parser.h"
#include <map>
#include <optional>
#include <string>

namespace mappers
{
class ReligionMapper: commonItems::parser
{
  public:
	ReligionMapper();
	explicit ReligionMapper(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> getEU4ReligionForCK3Religion(const std::string& ck3Religion) const;

  private:
	void registerKeys();

	std::map<std::string, std::string> ck3Toeu4ReligionMap;
};
} // namespace mappers

#endif // RELIGION_MAPPER_H
