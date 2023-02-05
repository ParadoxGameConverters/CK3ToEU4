#ifndef EU4_WAREHOUSE_H
#define EU4_WAREHOUSE_H
#include <set>

#include "Color.h"
#include "Magick++.h"
#include "Recolorer.h"

namespace CK3
{
class CoatOfArms;
class Emblem;
} // namespace CK3

namespace EU4
{
struct coloredImage
{
	std::optional<commonItems::Color> color1;
	std::optional<commonItems::Color> color2;
	std::optional<commonItems::Color> color3;
	std::string patternName;
	Magick::Image imageData;
	bool operator==(const coloredImage& rhs);
};

class Warehouse
{
  public:
	void loadImageFolders(const std::set<std::string>& sources) { imageFolders = sources; }
	void storeCoA(long long ID, Magick::Image image) { coaStorage.insert(std::pair(ID, image)); }
	[[nodiscard]] std::pair<bool, Magick::Image> getCoA(long long ID);

	[[nodiscard]] std::pair<Magick::Image, Magick::Image> getPattern(const CK3::CoatOfArms& coa); // colored/RAW (original) pattern
	[[nodiscard]] std::vector<std::pair<CK3::Emblem, Magick::Image>> getColoredTextures(const std::vector<CK3::Emblem>& emblems);
	[[nodiscard]] std::vector<std::pair<CK3::Emblem, Magick::Image>> getTexturedTextures(const std::vector<CK3::Emblem>& emblems);

  private:
	[[nodiscard]] std::vector<std::pair<CK3::Emblem, Magick::Image>> getTextures(const std::vector<CK3::Emblem>& emblems, const std::string& directoryName);

	std::set<std::string> imageFolders;

	std::map<std::string, Magick::Image> basePatterns; // these are raw background patterns and textured emblems
	std::map<std::string, Magick::Image> baseTextures; // these are raw colored emblems

	std::vector<coloredImage> patternStorage;	 // Storage for crafted CoA patterns, which are RGB solids
	std::vector<coloredImage> texturedStorage; // Storage for crafted colored emblems that use RGBA textures
	std::vector<coloredImage> coloredStorage;	 // Storage for crafted colored emblems, which are RGB solids

	std::map<long long, Magick::Image> coaStorage; // Storage for finished CoAs to be used as parents in other CoAs

	Recolorer recolorer;
};
} // namespace EU4

#endif // EU4_WAREHOUSE_H