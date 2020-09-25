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
	[[nodiscard]] static Magick::Image craftPatternImage(const coloredImage& imageBlock);
	[[nodiscard]] static Magick::Image craftTextureImage(const coloredImage& imageBlock);

  private:

	struct patternColorMasks // For reference only. Not used.
	{
		commonItems::Color color1 = commonItems::Color(std::array<int, 3>{255, 0, 0});
		commonItems::Color color2 = commonItems::Color(std::array<int, 3>{255, 255, 0});
		commonItems::Color color3 = commonItems::Color(std::array<int, 3>{255, 255, 255});
		commonItems::Color color4 = commonItems::Color(std::array<int, 3>{0, 0, 0}); // unknown
		commonItems::Color color5 = commonItems::Color(std::array<int, 3>{0, 0, 0}); // unknown
	} patternColorMasks;

	struct emblemColorMasks // For reference only. Not used.
	{
		commonItems::Color color1 = commonItems::Color(std::array<int, 3>{0, 0, 126});
		commonItems::Color color2 = commonItems::Color(std::array<int, 3>{0, 255, 126});
		commonItems::Color color3 = commonItems::Color(std::array<int, 3>{255, 0, 126});
	} emblemColorMasks;
};
} // namespace EU4

#endif // EU4_RECOLORER_H