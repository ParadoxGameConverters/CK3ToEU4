#ifndef CK3_LANDEDTITLES_H
#define CK3_LANDEDTITLES_H
#include "Color.h"
#include "Parser.h"
extern commonItems::Color::Factory laFabricaDeColor;

namespace CK3
{
class Title;
class ProvinceHolding;
class ProvinceHoldings;
class CountyDetails;
class CountyDetail;
class LandedTitles: commonItems::parser
{
  public:
	void loadTitles(std::istream& theStream);
	void loadTitles(const std::string& fileName);

	[[nodiscard]] auto isDefiniteForm() const { return definiteForm; }
	[[nodiscard]] auto isLandless() const { return landless; }
	[[nodiscard]] const auto& getColor() const { return color; }
	[[nodiscard]] const auto& getCapital() const { return capital; }
	[[nodiscard]] const auto& getProvince() const { return province; }
	[[nodiscard]] const auto& getCounty() const { return county; }
	[[nodiscard]] const auto& getFoundTitles() const { return foundTitles; }

	void loadProvinceHolding(const std::pair<int, std::shared_ptr<ProvinceHolding>>& provinceHolding) { province = provinceHolding; }
	void loadCountyDetails(const std::pair<std::string, std::shared_ptr<CountyDetail>>& countyDetail) { county = countyDetail; }

	void linkProvinceHoldings(const ProvinceHoldings& provinceHoldings);
	void linkCountyDetails(const CountyDetails& countyDetails);

  private:
	void registerKeys();

	bool definiteForm = false;
	bool landless = false;
	std::optional<commonItems::Color> color;
	std::pair<std::string, std::shared_ptr<Title>> capital;		  // This is of questionable use as savegame already defines defacto capitals.
	std::pair<int, std::shared_ptr<ProvinceHolding>> province;	  // only b_baronies have these - holdings are related to individual provinces on map.
	std::pair<std::string, std::shared_ptr<CountyDetail>> county; // only c_counties have these - these define common data for group of baronies under county.
	std::map<std::string, std::shared_ptr<LandedTitles>> foundTitles; // We're using title name, not savegame ID for key value.
};
} // namespace CK3

#endif // CK3_TITLES_H
