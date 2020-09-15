#ifndef EU4_WORLD_H
#define EU4_WORLD_H
#include "../CK3World/World.h"
#include "../Mappers/ConverterVersion/ConverterVersion.h"
#include "../Mappers/CultureMapper/CultureMapper.h"
#include "../Mappers/LocalizationMapper/LocalizationMapper.h"
#include "../Mappers/PrimaryTagMapper/PrimaryTagMapper.h"
#include "../Mappers/ProvinceMapper/ProvinceMapper.h"
#include "../Mappers/RegionMapper/RegionMapper.h"
#include "Output/outModFile.h"
#include "Country/Country.h"
#include "../Mappers/GovernmentsMapper/GovernmentsMapper.h"
#include "../Mappers/ReligionMapper/ReligionMapper.h"
#include "../Mappers/RulerPersonalitiesMapper/RulerPersonalitiesMapper.h"
#include "../Mappers/TitleTagMapper/TitleTagMapper.h"

class Configuration;

namespace CK3
{
} // namespace CK3

namespace EU4
{
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

	// output
	void output(const mappers::ConverterVersion& converterVersion, const Configuration& theConfiguration, const CK3::World& sourceWorld) const;
	void createModFile(const Configuration& theConfiguration) const;
	void outputVersion(const mappers::ConverterVersion& versionParser, const Configuration& theConfiguration) const;
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

	ModFile modFile;
};
} // namespace EU4

#endif // EU4_WORLD_H
