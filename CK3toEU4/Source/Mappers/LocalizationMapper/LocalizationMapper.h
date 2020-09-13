#ifndef LOCALIZATION_MAPPER
#define LOCALIZATION_MAPPER
#include <map>
#include <optional>
#include <string>

class Configuration;
namespace mappers
{
typedef struct
{
	std::string english;
	std::string french;
	std::string german;
	std::string spanish;
} LocBlock;

class LocalizationMapper
{
  public:
	LocalizationMapper() = default;
	void scrapeLocalizations(const Configuration& theConfiguration, const std::map<std::string, std::string>& mods);

	[[nodiscard]] std::optional<LocBlock> getLocBlockForKey(const std::string& key) const;

  private:
	void scrapeLanguage(const std::string& language, const std::string& path);
	void scrapeStream(std::istream& theStream, const std::string& language);
	
	std::map<std::string, LocBlock> localizations;
};
} // namespace mappers

#endif // LOCALIZATION_MAPPER