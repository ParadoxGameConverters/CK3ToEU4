#ifndef EU4_FLAG_CRAFTER_H
#define EU4_FLAG_CRAFTER_H
#include "Color.h"

namespace Magick
{
class Image;
} // namespace Magick

namespace CK3
{
class CoatOfArms;
class Emblem;
class EmblemInstance;
} // namespace CK3

namespace EU4
{
class Warehouse;
class FlagCrafter
{
  public:
	void loadWarehouse(const std::shared_ptr<Warehouse>& theWarehouse) { warehouse = theWarehouse; }

	[[nodiscard]] Magick::Image craftFlagFromCoA(const CK3::CoatOfArms& coa) const;

  private:
	[[nodiscard]] std::pair<Magick::Image, Magick::Image> processEmblemsOnImage(const std::pair<Magick::Image, Magick::Image>& imagePair,
		 const std::vector<std::pair<CK3::Emblem, Magick::Image>>& emblems) const;
	[[nodiscard]] std::pair<Magick::Image, Magick::Image> processSubsOnImage(const std::pair<Magick::Image, Magick::Image>& imagePair,
		 const std::vector<std::pair<CK3::CoatOfArms, Magick::Image>>& subs) const;
	[[nodiscard]] std::pair<Magick::Image, Magick::Image> imposeEmblemInstancesOnImage(const std::pair<Magick::Image, Magick::Image>& imagePair,
		 const std::vector<CK3::EmblemInstance>& instances,
		 const Magick::Image& emblem,
		 const std::vector<int>& masks) const;

	std::shared_ptr<Warehouse> warehouse;
};
} // namespace EU4

#endif // EU4_FLAG_CRAFTER_H