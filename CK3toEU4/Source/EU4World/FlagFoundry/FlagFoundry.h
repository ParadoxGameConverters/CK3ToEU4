#ifndef EU4_FLAG_FOUNDRY_H
#define EU4_FLAG_FOUNDRY_H
#include "CoatCrafter.h"
#include "Color.h"
#include <map>
#include <memory>
#include <string>

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
class FlagFoundry
{
  public:
	FlagFoundry();
	void generateFlags(const std::map<std::string, std::shared_ptr<Country>>& countries, const Configuration& theConfiguration);

  private:
	void craftFlag(const std::shared_ptr<Country>& country) const;

	std::string ck3Source; // path to ck3 installation gfx/coat_of_arms/ folder
	CoatCrafter coatCrafter;
};
} // namespace EU4

#endif // EU4_FLAG_FOUNDRY_H