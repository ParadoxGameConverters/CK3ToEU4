#ifndef RELIGION_MAPPER_H
#define RELIGION_MAPPER_H

#include "Parser.h"
#include <map>
#include <optional>
#include <string>
#include "../LocalizationMapper/LocalizationMapper.h"

namespace CK3
{
class Faiths;
class Faith;
} // namespace CK3

namespace mappers
{
class ReligionDefinitionMapper;
class ReligionMapper: commonItems::parser
{
  public:
	ReligionMapper();
	explicit ReligionMapper(std::istream& theStream);
	void importCK3Faiths(const CK3::Faiths& faiths, const ReligionDefinitionMapper& definitions);

	[[nodiscard]] std::optional<std::string> getEU4ReligionForCK3Religion(const std::string& ck3Religion) const;
	[[nodiscard]] const auto& getLocalizations() const { return localizations; }

  private:
	void registerKeys();
	void importCK3Faith(const CK3::Faith& faith, const ReligionDefinitionMapper& definitions);

	std::map<std::string, std::string> CK3toEU4ReligionMap;
	std::map<std::string, LocBlock> localizations;
};
} // namespace mappers

#endif // RELIGION_MAPPER_H
