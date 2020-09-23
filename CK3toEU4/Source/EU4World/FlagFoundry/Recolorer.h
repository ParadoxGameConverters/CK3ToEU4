#ifndef EU4_RECOLORER_H
#define EU4_RECOLORER_H
#include "Color.h"

namespace Magick
{
class Image;
} // namespace Magick

namespace EU4
{
struct coloredImage;
class Recolorer
{
  public:
	[[nodiscard]] Magick::Image craftPatternImage(const coloredImage& imageBlock) const;
	[[nodiscard]] Magick::Image craftTextureImage(const coloredImage& imageBlock) const;

  private:
	enum class IMAGETYPE
	{
		PATTERN,
		TEXTURE
	};
	enum class COLOR
	{
		COLOR1,
		COLOR2,
		COLOR3,
		COLOR4,
		COLOR5
	};

	[[nodiscard]] Magick::Image craftImage(const coloredImage& imageBlock, IMAGETYPE imageType) const;
	[[nodiscard]] Magick::Image recolorPattern(const Magick::Image& patternImage,
		 const std::vector<std::pair<COLOR, commonItems::Color>>& replacementMatrix) const;
	[[nodiscard]] Magick::Image recolorTexture(const Magick::Image& textureImage,
		 const std::vector<std::pair<COLOR, commonItems::Color>>& replacementMatrix) const;
	[[nodiscard]] Magick::Image recolorImage(const Magick::Image& image,
		 const std::vector<std::pair<commonItems::Color, commonItems::Color>>& replacementColors) const;

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

#endif // EU4_RECOLORER_H