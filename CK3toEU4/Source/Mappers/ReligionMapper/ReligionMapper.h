#ifndef RELIGION_MAPPER_H
#define RELIGION_MAPPER_H

#include "../../EU4World/Religion/GeneratedReligion.h"
#include "../LocalizationMapper/LocalizationMapper.h"
#include "Parser.h"
#include <map>
#include <optional>
#include <string>

namespace CK3
{
class Faiths;
class Faith;
} // namespace CK3

namespace mappers
{
class ReligionDefinitionMapper;
class ReligionGroupScraper;

class ReligionMapper: commonItems::parser
{
  public:
	ReligionMapper();
	explicit ReligionMapper(std::istream& theStream);
	void importCK3Faiths(const CK3::Faiths& faiths,
		 ReligionDefinitionMapper& religionDefinitionMapper,
		 const ReligionGroupScraper& religionGroupScraper,
		 const LocalizationMapper& localizationMapper);

	[[nodiscard]] std::optional<std::string> getEU4ReligionForCK3ReligiousHead(const std::string& ck3ReligiousHead) const;
	[[nodiscard]] std::optional<std::string> getEU4ReligionForCK3Religion(const std::string& ck3Religion) const;
	[[nodiscard]] std::optional<std::string> getEU4SchoolForCK3Religion(const std::string& ck3Religion) const;
	[[nodiscard]] const auto& getLocalizations() const { return localizations; }
	[[nodiscard]] const auto& getGeneratedReligions() const { return generatedReligions; }
	[[nodiscard]] const auto& getReformedReligions() const { return reformedReligions; }

  private:
	void registerKeys();
	void importCK3Faith(const CK3::Faith& faith,
		 ReligionDefinitionMapper& religionDefinitionMapper,
		 const ReligionGroupScraper& religionGroupScraper,
		 const LocalizationMapper& localizationMapper);

	std::map<std::string, std::pair<std::string, std::optional<std::string>>, std::pair<std::string, std::optional<std::string>>> CK3toEU4ReligionMap; // ck3 faith -> <eu4religion, eu4school>
	std::map<std::string, LocBlock> localizations;

	std::vector<EU4::GeneratedReligion> generatedReligions;
	std::vector<std::string> reformedReligions;
};
} // namespace mappers

#endif // RELIGION_MAPPER_H
