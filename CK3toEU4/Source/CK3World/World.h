#ifndef CK3_WORLD_H
#define CK3_WORLD_H

#include "../Mappers/IAmHreMapper/IAmHreMapper.h"
#include "../Mappers/NamedColors/NamedColors.h"
#include "../Mappers/ShatterEmpiresMapper/ShatterEmpiresMapper.h"
#include "../Mappers/TraitScraper/TraitScraper.h"
#include "Characters/Characters.h"
#include "CoatsOfArms/CoatsOfArms.h"
#include "Cultures/Cultures.h"
#include "Dynasties/Dynasties.h"
#include "Dynasties/Houses.h"
#include "Flags/Flags.h"
#include "GameVersion.h"
#include "Geography/CountyDetails.h"
#include "Geography/ProvinceHoldings.h"
#include "Mods/Mods.h"
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
	explicit World(const std::shared_ptr<Configuration>& theConfiguration);

	[[nodiscard]] const auto& getConversionDate() const { return endDate; }
	[[nodiscard]] const auto& getIndeps() const { return independentTitles; }
	[[nodiscard]] const auto& getMods() const { return mods; }
	[[nodiscard]] const auto& getTitles() const { return titles; }

  private:
	// savegame processing
	void verifySave(const std::string& saveGamePath);
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

	// CK2World processing
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

	date endDate = date("1444.11.11");
	date startDate = date("1.1.1");
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

	std::optional<std::pair<std::string, std::shared_ptr<Title>>> hreTitle; // loaded by configuration option.
	std::map<std::string, std::shared_ptr<Title>> independentTitles;

	enum class SaveType
	{
		INVALID = 0,
		ZIPFILE = 1,
		AUTOSAVE = 2,
		IRONMAN = 3
	};
	struct saveData
	{
		SaveType saveType = SaveType::INVALID;
		int zipStart = 0;
		std::string gamestate;
		std::string metadata; // probably not needed
	};
	saveData saveGame;
};
} // namespace CK3

#endif // CK3_WORLD_H
