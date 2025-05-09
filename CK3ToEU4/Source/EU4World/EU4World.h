#ifndef EU4_WORLD_H
#define EU4_WORLD_H
#include "../CK3World/Cultures/Culture.h"
#include "../CK3World/World.h"
#include "../Mappers/AfricanPassesMapper/AfricanPassesMapper.h"
#include "../Mappers/CultureDefinitionsMapper/CultureDefinitionsMapper.h"
#include "../Mappers/CultureMapper/CultureMapper.h"
#include "../Mappers/DevWeightsMapper/DevWeightsMapper.h"
#include "../Mappers/DynamicIdeasMapper/DynamicIdeasMapper.h"
#include "../Mappers/GovernmentsMapper/GovernmentsMapper.h"
#include "../Mappers/IslamOverrideMapper/IslamOverrideMapper.h"
#include "../Mappers/LocDegraderMapper/LocDegraderMapper.h"
#include "../Mappers/LocalizationMapper/LocalizationMapper.h"
#include "../Mappers/PrimaryTagMapper/PrimaryTagMapper.h"
#include "../Mappers/ProvinceMapper/ProvinceMapper.h"
#include "../Mappers/RegionMapper/RegionMapper.h"
#include "../Mappers/ReligionDefinitionMapper/ReligionDefinitionMapper.h"
#include "../Mappers/ReligionGroupScraper/ReligionGroupScraper.h"
#include "../Mappers/ReligionMapper/ReligionMapper.h"
#include "../Mappers/RulerPersonalitiesMapper/RulerPersonalitiesMapper.h"
#include "../Mappers/StartDateMapper/StartDateMapper.h"
#include "../Mappers/TitleTagMapper/TitleTagMapper.h"
#include "ConverterVersion.h"
#include "Country/Country.h"
#include "Diplomacy/Diplomacy.h"
#include "FlagFoundry/FlagFoundry.h"
#include "Ideas/NationalIdeas.h"
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
	World(const CK3::World& sourceWorld, const Configuration& theConfiguration, const commonItems::ConverterVersion& converterVersion);

  private:
	// import
	void verifyAllCountyMappings(const std::map<std::string, std::shared_ptr<CK3::Title>>& ck3Titles) const;
	void importVanillaCountries(const std::string& eu4Path, bool invasion);
	void loadCountriesFromSource(std::istream& theStream, const std::string& sourcePath, bool isVanillaSource);
	void importCK3Countries(const CK3::World& sourceWorld, Configuration::STARTDATE startDateOption, date startDate, bool dynasticNames);
	void importCK3Country(const std::pair<std::string, std::shared_ptr<CK3::Title>>& title,
		 const CK3::World& sourceWorld,
		 Configuration::STARTDATE startDateOption,
		 date startDate,
		 bool dynasticNames);
	void importVanillaProvinces(const std::string& eu4Path, bool invasion);
	void importCK3Provinces(Configuration::DISCOVEREDBY discoveredBy);
	[[nodiscard]] std::optional<std::pair<std::string, std::shared_ptr<CK3::Title>>> determineProvinceSource(
		 const std::map<std::string, std::shared_ptr<CK3::Title>>& ck3Titles) const;

	// processing
	void alterProvinceDevelopment(bool absoluteSwitch);
	[[nodiscard]] std::tuple<double, double, double> sumBaroniesForDevelopment(const std::map<long long, std::shared_ptr<CK3::Title>>& baronies) const;
	[[nodiscard]] double calculateProvinceDevFactor(const std::shared_ptr<Province>& province) const;

	void linkProvincesToCountries();
	void verifyCapitals();
	void verifyReligionsAndCultures();
	void assignAllCountryReforms();
	void generateNationalIdeasFromDynamicCultures(const CK3::Cultures& cultures);
	void importAdvisers();
	void resolvePersonalUnions();
	void distributeHRESubtitles(const Configuration& theConfiguration);
	void markHRETag(const Configuration& theConfiguration,
		 const std::pair<std::string, std::shared_ptr<CK3::Title>>& hreTitle,
		 const date& conversionDate,
		 date startDate,
		 Configuration::STARTDATE startDateOption);
	void setElectors();
	void setFreeCities();
	void distributeClaims(const Configuration& theConfiguration);
	void distributeDeadCores();
	void distributeForts();
	void fixTengri();
	void fixDuplicateNames();
	void siberianQuestion(const Configuration& theConfiguration);
	void africanQuestion();
	void mongolianQuestion();
	void religiousQuestion(bool doesIslamExist);
	void mergeConfederations(const CK3::Confederations& confederations);
	void coalesceHordes();

	// output
	void output(const commonItems::ConverterVersion& converterVersion,
		 const Configuration& theConfiguration,
		 const CK3::World& sourceWorld,
		 date startDate) const;
	void createModFile(const Configuration& theConfiguration) const;
	void outputVersion(const commonItems::ConverterVersion& versionParser, const Configuration& theConfiguration) const;
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
	void outputDynamicInstitutions(const Configuration& theConfiguration) const;
	void outputBookmark(const Configuration& theConfiguration, date conversionDate) const;
	void outputDynamicIdeasFile(const std::string& outputName) const;
	void outputReligions(const std::string& outputName,
		 const std::vector<GeneratedReligion>& generatedReligions,
		 const std::vector<std::string>& reformedReligions) const;
	void outputCultures(const std::string& outputName) const;
	void outputReligionIcons(const Configuration& theConfiguration, const std::vector<GeneratedReligion>& generatedReligions, const Mods& mods) const;
	void annexHordes(const std::string& tag,
		 std::map<std::string, std::vector<std::shared_ptr<Country>>>& hordeDependencies,
		 std::set<std::string> seenCountries);

	std::map<int, std::shared_ptr<Province>> provinces;
	std::map<std::string, std::shared_ptr<Country>> countries;
	std::set<std::string> specialCountryTags; // tags we loaded from own sources and must not output into 00_country_tags.txt
	std::vector<NationalIdeas> dynamicNationalIdeas;

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
	mappers::ReligionGroupScraper religionGroupScraper;
	mappers::IslamOverrideMapper islamOverrideMapper;
	mappers::LocDegraderMapper locDegrader;
	mappers::AfricanPassesMapper africanPassesMapper;
	mappers::CultureDefinitionsMapper cultureDefinitionsMapper;
	mappers::DynamicIdeasMapper dynamicIdeasMapper;
	mappers::StartDateMapper startDateMapper;

	ModFile modFile;
	Diplomacy diplomacy;
	FlagFoundry flagFoundry;

	std::string emperorTag;
	std::string actualHRETag;
};
} // namespace EU4

#endif // EU4_WORLD_H
