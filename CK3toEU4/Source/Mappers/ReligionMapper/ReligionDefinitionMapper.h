#ifndef RELIGION_DEFINITION_MAPPER_H
#define RELIGION_DEFINITION_MAPPER_H

#include "Parser.h"
#include <map>
#include <optional>
#include "ReligionDefinitionMapping.h"

namespace mappers
{
class ReligionDefinitionMapper: commonItems::parser
{
  public:
	ReligionDefinitionMapper();
	explicit ReligionDefinitionMapper(std::istream& theStream);

	[[nodiscard]] std::optional<ReligionDefinitionMapping> getDefinition(const std::string& definition) const;

  private:
	void registerKeys();

	std::map<std::string, ReligionDefinitionMapping> mappings;
};
} // namespace mappers

#endif // RELIGION_DEFINITION_MAPPER_H
