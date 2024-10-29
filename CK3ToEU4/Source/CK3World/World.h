#ifndef CK3_WORLD_H
#define CK3_WORLD_H
#include "../Mappers/CultureMapper/CultureMapper.cs"
#include "../Mappers/IAmHreMapper/IAmHreMapper.h"
#include "../Mappers/LocalizationMapper/LocalizationMapper.cs"
#include "../Mappers/NamedColors/NamedColors.cs"
#include "../Mappers/ShatterEmpiresMapper/ShatterEmpiresMapper.h"
#include "../Mappers/TraitScraper/TraitScraper.cs"
#include "../Mappers/VassalSplitoffMapper/VassalSplitoffMapper.h"
#include "Characters/Characters.h"
#include "CoatsOfArms/CoatsOfArms.h"
#include "ConverterVersion.h"
#include "Cultures/Cultures.h"
#include "Dynasties/Dynasties.h"
#include "Dynasties/HouseNameScraper.h"
#include "Dynasties/Houses.h"
#include "Flags/Flags.h"
#include "GameVersion.h"
#include "Geography/CountyDetails.h"
#include "Geography/ProvinceHoldings.h"
#include "ModLoader/ModLoader.h"
#include "Parser.h"
#include "Religions/Faiths.h"
#include "Religions/Religions.h"
#include "Titles/LandedTitles.h"
#include "Titles/Titles.h"
#include <Date.h>

class Configuration;

namespace CK3
{
class World: commonItems::parser
{
  public:
	explicit World(const std::shared_ptr<Configuration>& theConfiguration, const commonItems::ConverterVersion& converterVersion);

	[[nodiscard]] const auto& getConversionDate() const { return endDate; }
	[[nodiscard]] const auto& getIndeps() const { return independentTitles; }
	[[nodiscard]] const auto& getMods() const { return mods; }
	[[nodiscard]] const auto& getTitles() const { return titles; }
	[[nodiscard]] const auto& getFaiths() const { return faiths; }
	[[nodiscard]] const auto& getHRETitle() const { return hreTitle; }
	[[nodiscard]] auto doesIslamExist() const { return islamExists; }
	[[nodiscard]] auto isCoADesignerEnabled() const { return coaDesigner; }
	[[nodiscard]] const auto& getPlayerTitle() const { return playerTitle; }
	[[nodiscard]] const auto& getLocalizationMapper() const { return localizationMapper; }
	[[nodiscard]] const auto& getCultureMapper() const { return cultureMapper; }
	[[nodiscard]] const auto& getCultures() const { return cultures; }

  private:
	void registerKeys(const std::shared_ptr<Configuration>& theConfiguration, const commonItems::ConverterVersion& converterVersion);

	parser metaParser;
	parser metaPreParser;

	// savegame processing
	void verifySave(const std::string& saveGamePath) const;
	void processSave(const std::string& saveGamePath);

	// pre-parsing prep
	void primeLaFabricaDeColor(const Configuration& theConfiguration);
	void loadLandedTitles(const Configuration& theConfiguration);
	void loadCharacterTraits(const Configuration& theConfiguration);
	void loadHouseNames(const Configuration& theConfiguration);

	// postparsing weave
	void crosslinkDatabases();

	// CK3World processing
	void flagHREProvinces(const Configuration& theConfiguration);
	void shatterHRE(const Configuration& theConfiguration) const;
	void shatterEmpires(const Configuration& theConfiguration) const;
	void filterIndependentTitles();
	void splitVassals(const Configuration& theConfiguration);
	void gatherCourtierNames();
	void congregateDFCounties();
	void congregateDJCounties();
	void filterLandlessTitles();
	void setElectors();
	void checkForIslam();
	void locatePlayerTitle(const std::shared_ptr<Configuration>& theConfiguration);

	date endDate = date("1444.11.11");
	date startDate = date("1.1.1");
	std::optional<long long> playerID;
	std::optional<std::string> playerTitle;
	bool coaDesigner = false;
	GameVersion CK3Version;
	Titles titles;
	ProvinceHoldings provinceHoldings;
	Characters characters;
	Dynasties dynasties;
	Houses houses;
	Religions religions;
	Faiths faiths;
	CoatsOfArms coats;
	LandedTitles landedTitles;
	Mods mods;
	Flags flags;
	CountyDetails countyDetails;
	Cultures cultures;
	HouseNameScraper houseNameScraper;
	mappers::NamedColors namedColors;
	mappers::IAmHreMapper iAmHreMapper;
	mappers::ShatterEmpiresMapper shatterEmpiresMapper;
	mappers::TraitScraper traitScraper;
	mappers::VassalSplitoffMapper vassalSplitoffMapper;
	mappers::LocalizationMapper localizationMapper;
	mappers::CultureMapper cultureMapper;

	std::optional<std::pair<std::string, std::shared_ptr<Title>>> hreTitle; // loaded by configuration option.
	std::map<std::string, std::shared_ptr<Title>> independentTitles;

	bool islamExists = false;

	struct saveData
	{
		std::string gamestate;
		std::string metadata; // we use this to set up mods before main processing.
		bool parsedMeta = false;
	};
	saveData saveGame;
};
} // namespace CK3

#endif // CK3_WORLD_H
