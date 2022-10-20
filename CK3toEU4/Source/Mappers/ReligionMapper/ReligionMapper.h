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

	[[nodiscard]] std::optional<std::string> getEU4ReligionForCK3Religion(const std::string& ck3Religion, const std::string& ck3ReligiousHead) const;
	[[nodiscard]] std::optional<std::string> getEU4SchoolForCK3Religion(const std::string& ck3Religion) const;
	[[nodiscard]] const auto& getLocalizations() const { return localizations; }
	[[nodiscard]] const auto& getGeneratedReligions() const { return generatedReligions; }
	[[nodiscard]] const auto& getReformedReligions() const { return reformedReligions; }
	[[nodiscard]] const auto& getCK3ReligiousDoctrines() const { return CK3Doctrines; }

  private:
	void registerKeys();
	void importCK3Faith(const CK3::Faith& faith,
		 ReligionDefinitionMapper& religionDefinitionMapper,
		 const ReligionGroupScraper& religionGroupScraper,
		 const LocalizationMapper& localizationMapper);

	typedef struct eu4ReligionStruct
	{
		std::optional<std::string> eu4religion;
		std::optional<std::string> eu4school;
		std::optional<std::string> religiousHead;
	} eu4ReligionStruct;
	std::map<std::string, eu4ReligionStruct> CK3toEU4ReligionMap;			  // ck3 faith, EU4 Religion
	std::map<std::string, eu4ReligionStruct> ReligionHeadToEU4ReligionMap; // ck3 Religious Head Title, EU4 Religion
	std::map<std::string, LocBlock> localizations;

	std::vector<EU4::GeneratedReligion> generatedReligions;
	std::vector<std::string> reformedReligions;

	std::map<std::string, std::vector<std::string>> CK3Doctrines; // Used for certain government reforms
};
} // namespace mappers

#endif // RELIGION_MAPPER_H
