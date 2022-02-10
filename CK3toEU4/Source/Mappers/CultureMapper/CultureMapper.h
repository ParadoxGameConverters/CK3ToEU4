#ifndef CULTURE_MAPPER_H
#define CULTURE_MAPPER_H

#include "../RegionMapper/RegionMapper.h"
#include "CultureMappingRule.h"
#include "Parser.h"
#include <optional>
#include <string>

namespace CK3
{
class Culture;
}

namespace mappers
{
class CultureMapper: commonItems::parser
{
  public:
	CultureMapper() = default;
	explicit CultureMapper(std::istream& theStream);
	void initializeMapper();

	void loadRegionMapper(const std::shared_ptr<RegionMapper>& theRegionMapper);
	void storeCultures(const std::map<long long, std::shared_ptr<CK3::Culture>>& incCultures);

	[[nodiscard]] std::optional<std::string> cultureMatch(const std::string& ck3culture,
		 const std::string& eu4religion,
		 int eu4Province,
		 const std::string& eu4ownerTag) const;

	[[nodiscard]] std::optional<std::string> cultureRegionalMatch(const std::string& ck3culture,
		 const std::string& eu4religion,
		 int eu4Province,
		 const std::string& eu4ownerTag) const;

	[[nodiscard]] std::optional<std::string> cultureNonRegionalNonReligiousMatch(const std::string& ck3culture,
		 const std::string& eu4religion,
		 int eu4Province,
		 const std::string& eu4ownerTag) const;

	[[nodiscard]] std::optional<std::string> getTechGroup(const std::string& incEU4Culture) const;
	[[nodiscard]] std::optional<std::string> getGFX(const std::string& incEU4Culture) const;

	[[nodiscard]] const auto& getTargetCultures() const { return targetCultures; }
	[[nodiscard]] const auto& getSourceCultures() const { return sourceCultures; }
	[[nodiscard]] const auto& getCK3Cultures() const { return cultures; }

  private:
	void registerKeys();
	void buildCultureCaches();

	std::vector<CultureMappingRule> cultureMapRules;

	std::set<std::string> targetCultures;
	std::set<std::string> sourceCultures;
	std::map<long long, std::shared_ptr<CK3::Culture>> cultures;

	std::set<std::string> eu4Overrides; // cultures from ck3 we don't map but return as is - either eu4 ready cultures or dynamic ones.
};
} // namespace mappers

#endif // CULTURE_MAPPER_H