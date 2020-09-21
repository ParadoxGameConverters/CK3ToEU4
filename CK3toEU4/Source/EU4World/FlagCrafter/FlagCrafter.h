#ifndef EU4_FLAG_CRAFTER_H
#define EU4_FLAG_CRAFTER_H
#include <map>
#include <memory>
#include <string>

#include "Color.h"

class Configuration;
namespace Magick
{
class Image;
class ColorRGB;
}
namespace CK3
{
class CoatOfArms;
}
namespace EU4
{
class Country;
class FlagCrafter
{
  public:
	void generateFlags(const std::map<std::string, std::shared_ptr<Country>>& countries, const Configuration& theConfiguration);

  private:
	void craftFlag(const std::shared_ptr<Country>& country) const;
	[[nodiscard]] Magick::Image craftFlagFromCoA(const CK3::CoatOfArms& coa) const;
	bool ColorFuzzyEqual(const Magick::ColorRGB& a, const Magick::ColorRGB& b) const;

	std::string ck3Source; // path to ck3 installation gfx/coat_of_arms/ folder

	enum class COLOR
	{
		COLOR1,
		COLOR2,
		COLOR3,
		COLOR4,
		COLOR5
	};

	[[nodiscard]] Magick::Image recolorPattern(const Magick::Image& pattern, COLOR colorcode, const commonItems::Color& color) const;
	[[nodiscard]] Magick::Image recolorEmblem(const Magick::Image& emblem, COLOR colorcode, const commonItems::Color& color) const;
	[[nodiscard]] Magick::Image recolorImage(const Magick::Image& image, const commonItems::Color& mask, const commonItems::Color& color) const;

	struct patternColorMasks
	{
		commonItems::Color color1 = commonItems::Color(std::array<int, 3>{255, 0, 0});
		commonItems::Color color2 = commonItems::Color(std::array<int, 3>{255, 255, 0});
		commonItems::Color color3 = commonItems::Color(std::array<int, 3>{255, 255, 255});
		commonItems::Color color4 = commonItems::Color(std::array<int, 3>{0, 0, 0}); // unknown
		commonItems::Color color5 = commonItems::Color(std::array<int, 3>{0, 0, 0}); // unknown
	} patternColorMasks;
	
	struct emblemColorMasks
	{
		commonItems::Color color1 = commonItems::Color(std::array<int, 3>{0, 0, 126});
		commonItems::Color color2 = commonItems::Color(std::array<int, 3>{0, 255, 126});
		commonItems::Color color3 = commonItems::Color(std::array<int, 3>{255, 255, 126});
	} emblemColorMasks;
};
} // namespace EU4

#endif // EU4_FLAG_CRAFTER_H