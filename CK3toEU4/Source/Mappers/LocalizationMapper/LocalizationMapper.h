#ifndef LOCALIZATION_MAPPER
#define LOCALIZATION_MAPPER
#include "ModLoader/ModLoader.h"
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
	std::string korean;
	std::string russian;
	std::string simp_chinese;
	std::string spanish;
} LocBlock;

class LocalizationMapper
{
  public:
	LocalizationMapper() = default;
	void scrapeLocalizations(const Configuration& theConfiguration, const Mods& mods);
	void scrapeStream(std::istream& theStream, const std::string& language);

	[[nodiscard]] std::optional<LocBlock> getLocBlockForKey(const std::string& key) const;
	[[nodiscard]] std::optional<std::string> reverseLookupCultureName(const std::string& localization) const;

  private:
	void scrapeLanguage(const std::string& language, const std::string& path);

	[[nodiscard]] static std::pair<std::string, std::string> determineKeyLocalizationPair(const std::string& text);

	std::map<std::string, LocBlock> localizations;
};
} // namespace mappers

#endif // LOCALIZATION_MAPPER