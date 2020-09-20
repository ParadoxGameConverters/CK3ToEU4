#ifndef EU4_FLAG_CRAFTER_H
#define EU4_FLAG_CRAFTER_H
#include <map>
#include <memory>
#include <string>

class Configuration;
namespace EU4
{
class Country;
class FlagCrafter
{
  public:
	void generateFlags(const std::map<std::string, std::shared_ptr<Country>>& countries, const Configuration& theConfiguration);
	
  private:
	void craftFlag(const std::shared_ptr<Country>& country);
};
} // namespace EU4

#endif // EU4_FLAG_CRAFTER_H