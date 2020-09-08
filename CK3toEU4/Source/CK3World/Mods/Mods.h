#ifndef CK3_MODS_H
#define CK3_MODS_H

#include <map>
#include <optional>
#include <string>

class Configuration;
namespace CK3
{
class Mods
{
  public:
	Mods() = default;
	void loadModDirectory(const Configuration& theConfiguration);

	[[nodiscard]] const auto& getMods() const { return usableMods; }

  private:
	void loadCK3ModDirectory(const Configuration& theConfiguration);

	[[nodiscard]] bool extractZip(const std::string& archive, const std::string& path) const;
	[[nodiscard]] std::optional<std::string> getModPath(const std::string& modName) const;

	std::map<std::string, std::string> possibleMods;			  // name, absolute path to mod directory
	std::map<std::string, std::string> possibleCompressedMods; // name, absolute path to zip file
	std::map<std::string, std::string> usableMods;				  // name, absolute path for directories, relative for unpacked
};
} // namespace CK3

#endif // CK3_MODS_H