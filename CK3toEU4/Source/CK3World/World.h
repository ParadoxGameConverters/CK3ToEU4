#ifndef CK3_WORLD_H
#define CK3_WORLD_H
#include "../Mappers/IAmHreMapper/IAmHreMapper.h"
#include "../Mappers/NamedColors/NamedColors.h"
#include "../Mappers/ShatterEmpiresMapper/ShatterEmpiresMapper.h"
#include "../Mappers/TraitScraper/TraitScraper.h"
#include "../Mappers/VassalSplitoffMapper/VassalSplitoffMapper.h"
#include "Characters/Characters.h"
#include "CoatsOfArms/CoatsOfArms.h"
#include "ConverterVersion.h"
#include "Cultures/Cultures.h"
#include "Dynasties/Dynasties.h"
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
#include "../Mappers/LocalizationMapper/LocalizationMapper.h"
#include "../Mappers/CultureMapper/CultureMapper.h"
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

  private:
	// savegame processing
	void verifySave(const std::string& saveGamePath);
	void processRegularSave(const std::string& saveGamePath);
	void processCompressedSave(const std::string& saveGamePath);
	void processAutoSave(const std::string& saveGamePath);
	void processIronManSave(const std::string& saveGamePath);
	void processSave(const std::string& saveGamePath);

	// pre-parsing prep
	void primeLaFabricaDeColor(const Configuration& theConfiguration);
	void loadLandedTitles(const Configuration& theConfiguration);
	void loadCharacterTraits(const Configuration& theConfiguration);

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

	enum class SaveType
	{
		INVALID = 0,
		ZIPFILE = 1,
		AUTOSAVE = 2,
		IRONMAN = 3,
		REGULAR = 4
	};
	struct saveData
	{
		SaveType saveType = SaveType::INVALID;
		int zipStart = 0;
		std::string gamestate;
		std::string metadata; // we use this to set up mods before main processing.
	};
	saveData saveGame;
};
} // namespace CK3

#endif // CK3_WORLD_H
