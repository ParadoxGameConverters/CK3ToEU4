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

// Returns substring of str before nth occurrence of match string.
std::string getLeadStr(const std::string& str, const int occurrence = 1, const std::string& match = "_");
// Returns substring of string after nth occurrence of match string.
std::string getTailStr(const std::string& str, const int occurrence = 1, const std::string& match = "_");

class LocalizationMapper
{
  public:
	LocalizationMapper() = default;
	void scrapeLocalizations(const Configuration& theConfiguration, const Mods& mods);
	void scrapeStream(std::istream& theStream, const std::string& language);
	void unravelNestedLocs(LocBlock& block) const;

	[[nodiscard]] std::optional<LocBlock> getLocBlockForKey(const std::string& key) const;
	[[nodiscard]] std::optional<std::string> reverseLookupCultureName(const std::string& localization) const;

  private:
	void scrapeLanguage(const std::string& language, const std::string& path);
	const std::string selectLanguage(const std::string& language, const LocBlock& block) const;
	void assignSelectLanguage(const std::string& str, const std::string& language, LocBlock& block) const;

	[[nodiscard]] static std::pair<std::string, std::string> determineKeyLocalizationPair(const std::string& text);

	std::map<std::string, LocBlock> localizations;
};
} // namespace mappers

#endif // LOCALIZATION_MAPPER