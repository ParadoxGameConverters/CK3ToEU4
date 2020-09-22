#ifndef EU4_COAT_CRAFTER_H
#define EU4_COAT_CRAFTER_H
#include <memory>
#include "Color.h"

namespace Magick
{
class Image;
} // namespace Magick

namespace CK3
{
class CoatOfArms;
} // namespace CK3

enum class COLOR
{
	COLOR1,
	COLOR2,
	COLOR3,
	COLOR4,
	COLOR5
};

namespace EU4
{
class CoatCrafter
{
  public:
	[[nodiscard]] Magick::Image craftFlagFromCoA(const CK3::CoatOfArms& coa) const;

  private:

			struct patternColorMasks
	{
		commonItems::Color color1 = commonItems::Color(std::array<int, 3>{255, 0, 0});
		commonItems::Color color2 = commonItems::Color(std::array<int, 3>{255, 255, 0});
		commonItems::Color color3 = commonItems::Color(std::array<int, 3>{255, 255, 255});
		commonItems::Color color4 = commonItems::Color(std::array<int, 3>{0, 0, 0}); // unknown
		commonItems::Color color5 = commonItems::Color(std::array<int, 3>{0, 0, 0}); // unknown
	} patternColorMasks;

		std::map<std::string, Magick::Image> basePatterns; // these are raw background patterns and textured emblems
};
} // namespace EU4

#endif // EU4_COAT_CRAFTER_H