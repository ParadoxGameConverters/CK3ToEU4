#ifndef CK3_LANDEDTITLES_H
#define CK3_LANDEDTITLES_H
#include "Color.h"
#include "Parser.h"
extern commonItems::Color::Factory laFabricaDeColor;

namespace CK3
{
class Title;
class BaronyHolding;
class BaronyHoldings;
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
	[[nodiscard]] const auto& getFoundTitles() const { return foundTitles; }

	void loadBaronyHolding(const std::pair<int, std::shared_ptr<BaronyHolding>>& baronyHolding) { province = baronyHolding; }

	void linkBaronyHoldings(const BaronyHoldings& baronyHoldings);
	
  private:
	void registerKeys();

	bool definiteForm = false;
	bool landless = false;
	std::optional<commonItems::Color> color;
	std::pair<std::string, std::shared_ptr<Title>> capital;	// This is of questionable use as savegame already defines defacto capitals.
	std::pair<int, std::shared_ptr<BaronyHolding>> province; // province is area on map. b_ barony is its corresponding title.
	std::map<std::string, std::shared_ptr<LandedTitles>> foundTitles;			// We're using title name, not savegame ID for key value.
};
} // namespace CK3

#endif // CK3_TITLES_H
