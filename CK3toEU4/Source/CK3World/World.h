#ifndef CK3_WORLD_H
#define CK3_WORLD_H

#include "../Mappers/NamedColors/NamedColors.h"
#include "Characters/Characters.h"
#include "CoatsOfArms/CoatsOfArms.h"
#include "Dynasties/Dynasties.h"
#include "Dynasties/Houses.h"
#include "GameVersion.h"
#include "Geography/BaronyHoldings.h"
#include "Mods/Mods.h"
#include "Parser.h"
#include "Religions/Faiths.h"
#include "Religions/Religions.h"
#include "Titles/LandedTitles.h"
#include "Titles/Titles.h"
#include <Date.h>
#include "Flags/Flags.h"
#include "Geography/CountyDetails.h"

class Configuration;

namespace CK3
{
class World: commonItems::parser
{
  public:
	explicit World(const std::shared_ptr<Configuration>& theConfiguration);

	[[nodiscard]] const auto& getConversionDate() const { return endDate; }

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

	date endDate = date("1444.11.11");
	date startDate = date("1.1.1");
	GameVersion CK3Version;
	Titles titles;
	BaronyHoldings baronyHoldings;
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
	mappers::NamedColors namedColors;

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
