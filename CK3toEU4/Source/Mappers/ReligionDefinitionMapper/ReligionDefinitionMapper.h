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
	explicit ReligionDefinitionMapper(std::istream& theStream, std::istream& theStream2); // for testing

	[[nodiscard]] std::optional<ReligionDefinitionMapping> getDefinition(const std::string& definition) const;
	[[nodiscard]] std::optional<std::string> getStaticBlob(const std::string& religion) const;
	[[nodiscard]] auto getNextIcon() { return ++currentIcon; }

  private:
	void registerKeys();
	void registerStaticKeys();
	int currentIcon = 0;
	std::map<std::string, ReligionDefinitionMapping> mappings;
	std::map<std::string, std::string> staticStorage; // these are static string blobs we only store and regurgitate.
};
} // namespace mappers

#endif // RELIGION_DEFINITION_MAPPER_H
