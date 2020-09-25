#ifndef EU4_WORLD_H
#define EU4_WORLD_H
#include "../CK3World/World.h"
#include "../Mappers/ConverterVersion/ConverterVersion.h"
#include "../Mappers/CultureMapper/CultureMapper.h"
#include "../Mappers/DevWeightsMapper/DevWeightsMapper.h"
#include "../Mappers/GovernmentsMapper/GovernmentsMapper.h"
#include "../Mappers/LocalizationMapper/LocalizationMapper.h"
#include "../Mappers/PrimaryTagMapper/PrimaryTagMapper.h"
#include "../Mappers/ProvinceMapper/ProvinceMapper.h"
#include "../Mappers/RegionMapper/RegionMapper.h"
#include "../Mappers/ReligionMapper/ReligionDefinitionMapper.h"
#include "../Mappers/ReligionMapper/ReligionMapper.h"
#include "../Mappers/RulerPersonalitiesMapper/RulerPersonalitiesMapper.h"
#include "../Mappers/TitleTagMapper/TitleTagMapper.h"
#include "Country/Country.h"
#include "Diplomacy/Diplomacy.h"
#include "FlagFoundry/FlagFoundry.h"
#include "Output/outModFile.h"

class Configuration;

namespace CK3
{
} // namespace CK3

namespace EU4
{
class Province;
class World
{
  public:
	World(const CK3::World& sourceWorld, const Configuration& theConfiguration, const mappers::ConverterVersion& converterVersion);

  private:
	// import
	void importVanillaCountries(const std::string& eu4Path, bool invasion);
	void loadCountriesFromSource(std::istream& theStream, const std::string& sourcePath, bool isVanillaSource);
	void importCK3Countries(const CK3::World& sourceWorld);
	void importCK3Country(const std::pair<std::string, std::shared_ptr<CK3::Title>>& title, const CK3::World& sourceWorld);
	void importVanillaProvinces(const std::string& eu4Path, bool invasion);
	void importCK3Provinces(const CK3::World& sourceWorld);
	[[nodiscard]] std::optional<std::pair<std::string, std::shared_ptr<CK3::Title>>> determineProvinceSource(
		 const std::map<std::string, std::shared_ptr<CK3::Title>>& ck3Titles,
		 const CK3::World& sourceWorld) const;

	// processing
	void alterProvinceDevelopment();
	void linkProvincesToCountries();
	void verifyCapitals();
	void verifyReligionsAndCultures();
	void assignAllCountryReforms();
	void importAdvisers();
	void resolvePersonalUnions();
	void distributeHRESubtitles(const Configuration& theConfiguration);
	void setElectors();
	void setFreeCities();
	void distributeClaims(const Configuration& theConfiguration);
	void distributeDeadCores();
	void distributeForts();
	void fixTengri();
	void fixDuplicateNames();
	void siberianQuestion(const Configuration& theConfiguration);
	void africanQuestion();
	void indianQuestion();

	// output
	void output(const mappers::ConverterVersion& converterVersion, const Configuration& theConfiguration, const CK3::World& sourceWorld) const;
	void createModFile(const Configuration& theConfiguration) const;
	void outputVersion(const mappers::ConverterVersion& versionParser, const Configuration& theConfiguration) const;
	void outputCommonCountriesFile(const Configuration& theConfiguration) const;
	void outputCommonCountries(const Configuration& theConfiguration) const;
	void outputHistoryCountries(const Configuration& theConfiguration) const;
	void outputAdvisers(const Configuration& theConfiguration) const;
	void outputHistoryProvinces(const Configuration& theConfiguration) const;
	void outputLocalization(const Configuration& theConfiguration, bool invasion) const;
	void outputEmperor(const Configuration& theConfiguration, date conversionDate) const;
	void outputDiplomacy(const Configuration& theConfiguration, const std::vector<std::shared_ptr<Agreement>>& agreements, bool invasion) const;
	void outputFlags(const Configuration& theConfiguration) const;
	void outputInvasionExtras(const Configuration& theConfiguration) const;
	void outputBookmark(const Configuration& theConfiguration, date conversionDate) const;

	std::map<int, std::shared_ptr<Province>> provinces;
	std::map<std::string, std::shared_ptr<Country>> countries;
	std::set<std::string> specialCountryTags; // tags we loaded from own sources and must not output into 00_country_tags.txt

	mappers::LocalizationMapper localizationMapper;
	mappers::PrimaryTagMapper primaryTagMapper;
	std::shared_ptr<mappers::RegionMapper> regionMapper;
	mappers::CultureMapper cultureMapper;
	mappers::ProvinceMapper provinceMapper;
	mappers::GovernmentsMapper governmentsMapper;
	mappers::ReligionMapper religionMapper;
	mappers::TitleTagMapper titleTagMapper;
	mappers::RulerPersonalitiesMapper rulerPersonalitiesMapper;
	mappers::DevWeightsMapper devWeightsMapper;
	mappers::ReligionDefinitionMapper religionDefinitionMapper;

	ModFile modFile;
	Diplomacy diplomacy;
	FlagFoundry flagFoundry;

	std::string emperorTag;
};
} // namespace EU4

#endif // EU4_WORLD_H
