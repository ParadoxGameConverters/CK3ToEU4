#ifndef RELIGION_MAPPER_H
#define RELIGION_MAPPER_H

#include "../LocalizationMapper/LocalizationMapper.h"
#include "Color.h"
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

struct generatedReligion
{
	std::string country;
	std::string countrySecondary;
	std::string province;
	std::string unique;
	std::string nonUnique;
	std::string iconPath;
	std::string religionGroup;
	std::string staticBlob;
	std::string name;
	std::optional<commonItems::Color> color;
	int icon = 0;
};

class ReligionMapper: commonItems::parser
{
  public:
	ReligionMapper();
	explicit ReligionMapper(std::istream& theStream);
	void importCK3Faiths(const CK3::Faiths& faiths, ReligionDefinitionMapper& religionDefinitionMapper, const ReligionGroupScraper& religionGroupScraper);

	[[nodiscard]] std::optional<std::string> getEU4ReligionForCK3Religion(const std::string& ck3Religion) const;
	[[nodiscard]] const auto& getLocalizations() const { return localizations; }
	[[nodiscard]] const auto& getGeneratedReligions() const { return generatedReligions; }

  private:
	void registerKeys();
	void importCK3Faith(const CK3::Faith& faith, ReligionDefinitionMapper& religionDefinitionMapper, const ReligionGroupScraper& religionGroupScraper);

	std::map<std::string, std::string> CK3toEU4ReligionMap;
	std::map<std::string, LocBlock> localizations;

	std::vector<generatedReligion> generatedReligions;
};
} // namespace mappers

#endif // RELIGION_MAPPER_H
