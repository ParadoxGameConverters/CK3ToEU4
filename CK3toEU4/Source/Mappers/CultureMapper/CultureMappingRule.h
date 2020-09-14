#ifndef CULTURE_MAPPING_RULE_H
#define CULTURE_MAPPING_RULE_H

#include "Parser.h"
#include <set>

namespace mappers
{
class RegionMapper;
class CultureMappingRule: commonItems::parser
{
  public:
	CultureMappingRule() = default;
	explicit CultureMappingRule(std::istream& theStream);

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

	void insertRegionMapper(const std::shared_ptr<RegionMapper>& theRegionMapper) { regionMapper = theRegionMapper; }

	[[nodiscard]] const auto& getEU4Culture() const { return destinationCulture; } // for testing
	[[nodiscard]] const auto& getCK3Cultures() const { return cultures; }			 // for testing
	[[nodiscard]] const auto& getReligions() const { return religions; }				 // for testing
	[[nodiscard]] const auto& getRegions() const { return regions; }					 // for testing
	[[nodiscard]] const auto& getOwners() const { return owners; }						 // for testing
	[[nodiscard]] const auto& getProvinces() const { return provinces; }				 // for testing
	[[nodiscard]] const auto& getTechGroupField() const { return techGroup; }		 // for testing
	[[nodiscard]] const auto& getGFXField() const { return gfx; }						 // for testing

  private:
	void registerKeys();
	
	std::string destinationCulture;
	std::string techGroup;
	std::string gfx;
	std::set<std::string> cultures;
	std::set<std::string> religions;
	std::set<std::string> regions;
	std::set<std::string> owners;
	std::set<int> provinces;

	std::shared_ptr<RegionMapper> regionMapper;
};
} // namespace mappers

#endif // CULTURE_MAPPING_RULE_H
