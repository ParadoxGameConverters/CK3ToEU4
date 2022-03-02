#ifndef CULTURE_DEFINITIONS_MAPPER_H
#define CULTURE_DEFINITIONS_MAPPER_H
#include "../HeritageMapper/HeritageMapper.h"
#include "Parser.h"

namespace CK3
{
class Culture;
}

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

	void loadDefinitionsFromEU4Installation(const Configuration& theConfiguration);
	void buildDefinitions(const CultureMapper& cultureMapper);
	void loadHeritagesFromDisk() { herritageMapper.loadHeritagesFromDisk(); }
	void loadHeritages(const HeritageMapper& heritages) { herritageMapper = heritages; }

	[[nodiscard]] std::shared_ptr<CultureGroupDefinition> getGroupForCulture(const std::string& cultureName) const;
	[[nodiscard]] std::shared_ptr<CultureDefinition> getCulture(const std::string& cultureName) const;
	[[nodiscard]] std::shared_ptr<CultureGroupDefinition> getCultureGroup(const std::string& cultureGroupName);
	[[nodiscard]] std::shared_ptr<CultureGroupDefinition> getCultureGroupForCultureName(const std::string& cultureName) const;
	[[nodiscard]] const auto& getCultureGroupsMap() const { return cultureGroupsMap; }

	friend std::ostream& operator<<(std::ostream& output, const CultureDefinitionsMapper& cultureDefinitionsMapper);

  private:
	void registerKeys();
	void assignNamesToCulture(const std::shared_ptr<CultureDefinition>& sourceCulture,
		 const std::shared_ptr<CultureDefinition>& targetDefinition,
		 const std::string& sourceCultureName) const;
	void assignCultureToCultureGroup(const std::shared_ptr<CK3::Culture>& sourceCulture, const std::shared_ptr<CultureDefinition>& targetDefinition);

	HeritageMapper herritageMapper;

	std::map<std::string, std::shared_ptr<CultureGroupDefinition>> cultureGroupsMap;
};
} // namespace mappers

#endif // CULTURE_DEFINITIONS_MAPPER_H
