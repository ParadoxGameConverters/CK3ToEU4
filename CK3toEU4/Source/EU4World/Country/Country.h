#ifndef EU4_COUNTRY_H
#define EU4_COUNTRY_H

#include "../../Configuration/Configuration.h"
#include "../../Mappers/LocalizationMapper/LocalizationMapper.h"
#include "CountryDetails.h"
#include <memory>
#include <string>

namespace CK3
{
class Title;
}

namespace mappers
{
class GovernmentsMapper;
class ReligionMapper;
class RegionMapper;
class CultureMapper;
class ColorScraper;
class ProvinceMapper;
class RulerPersonalitiesMapper;
} // namespace mappers

namespace EU4
{
class Province;
class Country
{
  public:
	Country() = default;

	Country(std::string theTag, const std::string& filePath);
	void loadHistory(const std::string& filePath);
	void initializeFromTitle(const std::string& theTag,
		 const std::pair<std::string, std::shared_ptr<CK3::Title>>& theTitle,
		 const mappers::GovernmentsMapper& governmentsMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const mappers::CultureMapper& cultureMapper,
		 const mappers::ProvinceMapper& provinceMapper,
		 const mappers::LocalizationMapper& localizationMapper,
		 const mappers::RulerPersonalitiesMapper& rulerPersonalitiesMapper,
		 date theConversionDate,
		 Configuration::STARTDATE startDateOption);
	void setSunsetCountry(bool isSunsetCountry) { details.isSunsetCountry = isSunsetCountry; }
	void clearHistoryLessons() { details.historyLessons.clear(); }
	void registerProvince(std::pair<int, std::shared_ptr<Province>> theProvince) { provinces.insert(std::move(theProvince)); }
	bool verifyCapital(const mappers::ProvinceMapper& provinceMapper);
	void clearExcommunicated() { details.excommunicated = false; }
	void setPrimaryCulture(const std::string& culture);
	void setMajorityReligion(const std::string& religion);
	void setAcceptedCultures();
	void setReligion(const std::string& religion);
	void setTechGroup(const std::string& tech) { details.technologyGroup = tech; }
	void setGFX(const std::string& gfx) { details.graphicalCulture = gfx; }
	void assignReforms(const std::shared_ptr<mappers::RegionMapper>& regionMapper);
	void initializeAdvisers(const mappers::LocalizationMapper& localizationMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const mappers::CultureMapper& cultureMapper);
	void annexCountry(const std::pair<std::string, std::shared_ptr<Country>>& theCountry);
	void clearProvinces() { provinces.clear(); }
	void setElector() { details.elector = true; }
	void overrideReforms(const std::string& reform) { details.reforms = {reform}; }
	void setMercantilism(int mercantilism) { details.mercantilism = mercantilism; }
	void setGovernment(const std::string& government) { details.government = government; }
	void setLocalizations(const mappers::LocBlock& newBlock);
	void correctRoyaltyToBuddhism();

	[[nodiscard]] const auto& getCommonCountryFile() const { return commonCountryFile; }
	[[nodiscard]] const auto& getHistoryCountryFile() const { return historyCountryFile; }
	[[nodiscard]] const auto& getLocalizations() const { return localizations; }
	[[nodiscard]] const auto& getPrimaryCulture() const { return details.primaryCulture; }
	[[nodiscard]] const auto& getMajorityReligion() const { return details.majorityReligion; }
	[[nodiscard]] const auto& getReligion() const { return details.religion; }
	[[nodiscard]] const auto& getTechGroup() const { return details.technologyGroup; }
	[[nodiscard]] const auto& getGFX() const { return details.graphicalCulture; }
	[[nodiscard]] const auto& getProvinces() const { return provinces; }
	[[nodiscard]] const auto& getTitle() const { return title; }
	[[nodiscard]] const auto& getGovernment() const { return details.government; }
	[[nodiscard]] const auto& getGovernmentReforms() const { return details.reforms; }
	[[nodiscard]] const auto& getTag() const { return tag; }
	[[nodiscard]] const auto& getAdvisers() const { return details.advisers; }
	[[nodiscard]] auto getConversionDate() const { return conversionDate; }
	[[nodiscard]] auto isExcommunicated() const { return details.excommunicated; }
	[[nodiscard]] auto isHREEmperor() const { return details.holyRomanEmperor; }
	[[nodiscard]] auto isHREElector() const { return details.elector; }
	[[nodiscard]] auto isinHRE() const { return details.inHRE; }
	[[nodiscard]] auto isSunsetCountry() const { return details.isSunsetCountry; }
	[[nodiscard]] auto getCapitalID() const { return details.capital; }
	[[nodiscard]] auto getHouse() const { return details.house; }
	[[nodiscard]] auto getHasDynastyName() const { return details.hasDynastyName; }

	[[nodiscard]] int getDevelopment() const;

	void outputCommons(std::ostream& output) const;
	void outputAdvisers(std::ostream& output) const;

	friend std::ostream& operator<<(std::ostream& output, const Country& country);

  private:
	void populateHistory(const mappers::GovernmentsMapper& governmentsMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const mappers::ProvinceMapper& provinceMapper,
		 const mappers::CultureMapper& cultureMapper);
	void populateCommons(const mappers::CultureMapper& cultureMapper, const mappers::LocalizationMapper& localizationMapper);
	void populateMisc();
	void populateLocs(const mappers::LocalizationMapper& localizationMapper);
	void populateRulers(const mappers::ReligionMapper& religionMapper,
		 const mappers::CultureMapper& cultureMapper,
		 const mappers::RulerPersonalitiesMapper& rulerPersonalitiesMapper,
		 const mappers::LocalizationMapper& localizationMapper,
		 Configuration::STARTDATE startDateOption,
		 const date& theConversionDate);

	[[nodiscard]] date normalizeDate(const date& incomingDate,
		 Configuration::STARTDATE startDateOption,
		 const date& theConversionDate) const; // Uses bookmark date to shift dates if required.

	std::string tag;
	std::string commonCountryFile;
	std::string historyCountryFile;
	date conversionDate; // for dating the monarchs in history file.
	CountryDetails details;

	std::optional<std::pair<std::string, std::shared_ptr<CK3::Title>>> title;
	std::map<std::string, mappers::LocBlock> localizations; // Beware, these are UTF8 strings. If you are altering them be sure you know what you're doing.
	std::map<int, std::shared_ptr<Province>> provinces;
};
} // namespace EU4

#endif // EU4_COUNTRY_H
