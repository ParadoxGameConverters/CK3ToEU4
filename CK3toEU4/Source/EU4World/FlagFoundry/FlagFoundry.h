#ifndef EU4_FLAG_FOUNDRY_H
#define EU4_FLAG_FOUNDRY_H
#include "Color.h"
#include "FlagCrafter.h"
#include <map>
#include <memory>
#include <string>

class Configuration;

namespace Magick
{
class Image;
} // namespace Magick

namespace CK3
{
class CoatOfArms;
class Emblem;
class EmblemInstance;
class Mods;
} // namespace CK3

namespace EU4
{
class Country;
class Warehouse;
struct GeneratedReligion;
class FlagFoundry
{
  public:
	FlagFoundry();
	void generateFlags(const std::map<std::string, std::shared_ptr<Country>>& countries,
		 const Configuration& theConfiguration,
		 const std::vector<EU4::GeneratedReligion>& religions) const;
	void loadImageFolders(const Configuration& theConfiguration, const CK3::Mods& mods) const;
	void extendReligionStrips(const Configuration& theConfiguration, const std::vector<GeneratedReligion>& religions) const;

  private:
	void craftFlag(const std::shared_ptr<Country>& country) const;
	void craftRebelFlag(const Configuration& theConfiguration, const GeneratedReligion& religion) const;
	[[nodiscard]] Magick::Image extendReligionStrip(const Magick::Image& sourceStrip, const Magick::Image& icon) const;

	FlagCrafter flagCrafter;				  // image processor
	std::shared_ptr<Warehouse> warehouse; // background image storage, loading and preparation
};
} // namespace EU4

#endif // EU4_FLAG_FOUNDRY_H