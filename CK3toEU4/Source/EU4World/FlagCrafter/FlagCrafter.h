#ifndef EU4_FLAG_CRAFTER_H
#define EU4_FLAG_CRAFTER_H
#include <map>
#include <memory>
#include <string>

class Configuration;
namespace Magick
{
class Image;
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
	void generateFlags(const std::map<std::string, std::shared_ptr<Country>>& countries, const Configuration& theConfiguration) const;
	
  private:
	void craftFlag(const std::shared_ptr<Country>& country) const;
	[[nodiscard]] Magick::Image craftFlagFromCoA(const CK3::CoatOfArms& coa) const;
};
} // namespace EU4

#endif // EU4_FLAG_CRAFTER_H