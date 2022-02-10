#ifndef CULTURE_DEFINITIONS_MAPPER_H
#define CULTURE_DEFINITIONS_MAPPER_H
#include "Parser.h"

class Configuration;

namespace mappers
{
class CultureMapper;
class CultureGroupDefinition;
class CultureDefinition;
class CultureDefinitionsMapper: commonItems::parser
{
  public:
	CultureDefinitionsMapper() = default;
	explicit CultureDefinitionsMapper(std::istream& theStream);

	void initForEU4(const Configuration& theConfiguration);
	void buildDefinitions(const CultureMapper& cultureMapper);

	[[nodiscard]] std::shared_ptr<CultureGroupDefinition> getGroupForCulture(const std::string& cultureName) const;
	[[nodiscard]] std::shared_ptr<CultureDefinition> getCulture(const std::string& cultureName) const;
	[[nodiscard]] std::shared_ptr<CultureGroupDefinition> getCultureGroup(const std::string& cultureGroupName);
	[[nodiscard]] const auto& getCultureGroupsMap() const { return cultureGroupsMap; }

	friend std::ostream& operator<<(std::ostream& output, const CultureDefinitionsMapper& cultureDefinitionsMapper);

  private:
	void registerKeys();

	std::map<std::string, std::shared_ptr<CultureGroupDefinition>> cultureGroupsMap;
};
} // namespace mappers

#endif // CULTURE_DEFINITIONS_MAPPER_H
