#ifndef EU4_COUNTRY_H
#define EU4_COUNTRY_H

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
		 date theConversionDate);
	void setSunsetCountry(bool isSunsetCountry) { details.isSunsetCountry = isSunsetCountry; }
	void clearHistoryLessons() { details.historyLessons.clear(); }

	[[nodiscard]] const auto& getTitle() const { return title; }
	[[nodiscard]] const auto& getProvinces() const { return provinces; }
	[[nodiscard]] const auto& getConversionDate() const { return conversionDate; }

	friend std::ostream& operator<<(std::ostream& output, const Country& versionParser);

  private:
	std::string tag;
	std::string commonCountryFile;
	std::string historyCountryFile;
	date conversionDate; // for dating the monarchs in history file.
	CountryDetails details;

	std::optional<std::pair<std::string, std::shared_ptr<CK3::Title>>> title;
	std::map<std::string, mappers::LocBlock> localizations;
	std::map<int, std::shared_ptr<Province>> provinces;
};
} // namespace EU4

#endif // EU4_COUNTRY_H
