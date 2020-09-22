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
} // namespace Magick

namespace CK3
{
class CoatOfArms;
class Emblem;
class EmblemInstance;
} // namespace CK3

namespace EU4
{
class Country;
class FlagCrafter
{
  public:
	FlagCrafter();
	void generateFlags(const std::map<std::string, std::shared_ptr<Country>>& countries, const Configuration& theConfiguration);

  private:
	void craftFlag(const std::shared_ptr<Country>& country) const;
	[[nodiscard]] Magick::Image craftFlagFromCoA(const CK3::CoatOfArms& coa) const;

	std::string ck3Source; // path to ck3 installation gfx/coat_of_arms/ folder

	enum class COLOR
	{
		COLOR1,
		COLOR2,
		COLOR3,
		COLOR4,
		COLOR5
	};

	[[nodiscard]] Magick::Image recolorPattern(const Magick::Image& pattern, const std::vector<std::pair<COLOR, commonItems::Color>>& replacementMatrix) const;
	[[nodiscard]] Magick::Image recolorEmblem(const Magick::Image& emblem, const std::vector<std::pair<COLOR, commonItems::Color>>& replacementMatrix) const;
	[[nodiscard]] Magick::Image recolorImage(const Magick::Image& image,
		 const std::vector<std::pair<commonItems::Color, commonItems::Color>>& replacementColors) const;
	[[nodiscard]] Magick::Image craftPatternImage(const CK3::CoatOfArms& coa) const;
	[[nodiscard]] std::vector<std::pair<CK3::Emblem, Magick::Image>> craftEmblemImages(const std::vector<CK3::Emblem>& emblems) const;
	[[nodiscard]] Magick::Image processEmblemsOnImage(const Magick::Image& image, const std::vector<std::pair<CK3::Emblem, Magick::Image>>&emblems) const;
	[[nodiscard]] Magick::Image imposeEmblemInstancesOnImage(const Magick::Image& image,
		 const std::vector<CK3::EmblemInstance>& instances,
		 const Magick::Image& emblem) const;

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