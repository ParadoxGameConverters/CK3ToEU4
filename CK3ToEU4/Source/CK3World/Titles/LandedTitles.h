#ifndef CK3_LANDEDTITLES_H
#define CK3_LANDEDTITLES_H
#include "Color.h"
#include "Parser.h"
extern commonItems::Color::Factory laFabricaDeColor;

namespace CK3
{
class Title;
class Titles;
class ProvinceHolding;
class ProvinceHoldings;
class CountyDetails;
class CountyDetail;
class LandedTitles: commonItems::parser
{
  public:
	void loadTitles(std::istream& theStream);
	void loadTitles(const std::filesystem::path& fileName);

	[[nodiscard]] auto isDefiniteForm() const { return definiteForm; }
	[[nodiscard]] auto isLandless() const { return landless; }
	[[nodiscard]] auto canBeNamedAfterDynasty() const { return mayBeNamedAfterDynasty; }
	[[nodiscard]] const auto& getColor() const { return color; }
	[[nodiscard]] const auto& getCapital() const { return capital; }
	[[nodiscard]] const auto& getProvince() const { return province; }
	[[nodiscard]] const auto& getCounty() const { return county; }
	[[nodiscard]] const auto& getFoundTitles() const { return foundTitles; }

	void loadProvinceHolding(const std::pair<int, std::shared_ptr<ProvinceHolding>>& provinceHolding) { province = provinceHolding; }
	void loadCountyDetails(const std::pair<std::string, std::shared_ptr<CountyDetail>>& countyDetail) { county = countyDetail; }
	void loadCapital(const std::pair<std::string, std::shared_ptr<Title>>& theCapital) { capital = theCapital; }

	void linkProvinceHoldings(const ProvinceHoldings& provinceHoldings);
	void linkCountyDetails(const CountyDetails& countyDetails);
	void linkTitles(const Titles& titles);

  private:
	void registerKeys();

	bool definiteForm = false;
	bool landless = false;
	bool mayBeNamedAfterDynasty = true;
	std::optional<commonItems::Color> color;

	// This is of questionable use as savegame already defines defacto capitals. Not always present, and if present then a COUNTY.
	std::optional<std::pair<std::string, std::shared_ptr<Title>>> capital;

	// only b_baronies have these - holdings are related to individual provinces on map.
	std::optional<std::pair<int, std::shared_ptr<ProvinceHolding>>> province;

	// only c_counties have these - these define common data for group of baronies under county.
	std::optional<std::pair<std::string, std::shared_ptr<CountyDetail>>> county;

	// We're using title name, not savegame ID for key value.
	std::map<std::string, std::shared_ptr<LandedTitles>> foundTitles;
};
} // namespace CK3

#endif // CK3_TITLES_H
