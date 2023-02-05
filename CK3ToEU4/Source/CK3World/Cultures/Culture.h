#ifndef CK3_CULTURE_H
#define CK3_CULTURE_H
#include "../../Mappers/LocalizationMapper/LocalizationMapper.h"
#include "Parser.h"
#include <set>

namespace mappers
{
class CultureMapper;
}

namespace CK3
{
class Culture: commonItems::parser
{
  public:
	Culture() = default;
	Culture(std::istream& theStream, long long theID);

	[[nodiscard]] auto getID() const { return ID; }
	[[nodiscard]] auto isEU4Ready() const { return eu4Ready; }
	[[nodiscard]] auto isDynamic() const { return dynamic; }
	[[nodiscard]] const auto& getLocalizedName() const { return localizedName; }
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getNameLists() const { return nameLists; }
	[[nodiscard]] const auto& getHeritage() const { return heritage; }
	[[nodiscard]] const auto& getTemplate() const { return culture_template; }
	[[nodiscard]] const auto& getEthos() const { return ethos; }
	[[nodiscard]] const auto& getTraditions() const { return traditions; }

	void setDynamic() { dynamic = true; }
	void concoctCultureName(const mappers::LocalizationMapper& localizationMapper,
		 const mappers::CultureMapper& cultureMapper,
		 std::map<std::string, int>& cultureNamingCounter);

  private:
	void registerKeys();

	long long ID = 0;
	bool eu4Ready = false; // this culture has eu4 match and needs zero processing
	bool dynamic = false;  // this culture is dynamic and will need generation of cultural data

	std::optional<std::string> culture_template; // this has data only for base ck3 cultures, like czech or german
	std::optional<std::string> localizedName;		// this can be anything - user input or localized name in a particular language game is running.
	std::string heritage;								// all cultures should have this.
	std::set<std::string> nameLists;					// We use these to generate dynamic culture code names, in lack of a better solution.
	std::string ethos;									// used to generate custom ideas for custom tags with a custom culture
	std::vector<std::string> traditions;			// used to generate custom ideas for custom tags with a custom culture

	std::string name; // calculated value from all of the above - can be either *eu4* culture, ck3 vanilla, or anything in between.
};
} // namespace CK3

#endif // CK3_CULTURE_H
