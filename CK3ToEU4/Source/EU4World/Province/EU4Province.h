#ifndef EU4_PROVINCE_H
#define EU4_PROVINCE_H

#include "../../Configuration/Configuration.h"
#include "../../Mappers/LocDegraderMapper/LocDegraderMapper.h"
#include "../../Mappers/RegionMapper/RegionMapper.h"
#include "ProvinceDetails.h"
#include <memory>
#include <string>

namespace CK3
{
class Title;
}
namespace mappers
{
class CultureMapper;
class ReligionMapper;
} // namespace mappers

namespace EU4
{
class Country;
class Province
{
  public:
	Province() = default;
	Province(const std::shared_ptr<CK3::Title>& origProvince); // For tests

	Province(int id, const std::string& filePath);

	void updateWith(const std::string& filePath);
	void initializeFromCK3Title(const std::shared_ptr<CK3::Title>& origProvince,
		 const mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const mappers::LocDegraderMapper& locDegrader,
		 const std::shared_ptr<mappers::RegionMapper>& regionMapper,
		 Configuration::DISCOVEREDBY discoveredBy);
	void registerManualName(const mappers::LocDegraderMapper& locDegrader);

	[[nodiscard]] const auto& getHistoryCountryFile() const { return historyProvincesFile; }
	[[nodiscard]] const auto& getTagCountry() const { return tagCountry; }
	[[nodiscard]] const auto& getOwner() const { return details.owner; }
	[[nodiscard]] const auto& getReligion() const { return details.religion; }
	[[nodiscard]] const auto& getCulture() const { return details.culture; }
	[[nodiscard]] const auto& getSourceProvince() const { return srcProvince; }
	[[nodiscard]] const auto& getCenterOfTradeLevel() const { return details.centerOfTrade; }
	[[nodiscard]] const auto& getCustomName() const { return details.customName.value(); }
	[[nodiscard]] const bool isRenamed() const { return details.customName.has_value(); }
	[[nodiscard]] auto getDev() const { return details.baseTax + details.baseProduction + details.baseManpower; }
	[[nodiscard]] auto getAdm() const { return details.baseTax; }
	[[nodiscard]] auto getMil() const { return details.baseManpower; }
	[[nodiscard]] auto getDip() const { return details.baseProduction; }
	[[nodiscard]] auto getProvinceID() const { return provID; }

	void registerTagCountry(const std::pair<std::string, std::shared_ptr<Country>>& theCountry) { tagCountry = theCountry; }
	void addCore(const std::string& tag) { details.cores.insert(tag); }
	void dropCores() { details.cores.clear(); }
	void addClaim(const std::string& tag) { details.claims.insert(tag); }
	void addPermanentClaim(const std::string& tag) { details.permanentClaims.insert(tag); }
	void setOwner(const std::string& tag) { details.owner = tag; }
	void setController(const std::string& tag) { details.controller = tag; }
	void setReligion(const std::string& religion) { details.religion = religion; }
	void setAdm(int adm) { details.baseTax = adm; }
	void setDip(int dip) { details.baseProduction = dip; }
	void setMil(int mil) { details.baseManpower = mil; }
	void buildFort() { details.fort = true; }
	void addDiscoveredBy(const std::string& bywhom) { details.discoveredBy.insert(bywhom); }
	void sterilize();

	friend std::ostream& operator<<(std::ostream& output, const Province& province);

  private:
	int provID = 0;
	std::string historyProvincesFile;
	std::shared_ptr<CK3::Title> srcProvince;
	ProvinceDetails details;
	std::pair<std::string, std::shared_ptr<Country>> tagCountry;
};
} // namespace EU4

#endif // EU4_PROVINCE_H