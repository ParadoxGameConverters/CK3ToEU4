#ifndef CK3_WORLD_H
#define CK3_WORLD_H

#include "GameVersion.h"
#include "Geography/BaronyHoldings.h"
#include "Parser.h"
#include "Titles/Titles.h"
#include <Date.h>
#include "Characters/Characters.h"

class Configuration;

namespace CK3
{
class World: commonItems::parser
{
  public:
	explicit World(const Configuration& theConfiguration);

	[[nodiscard]] const auto& getConversionDate() const { return endDate; }

  private:
	void verifySave(const std::string& saveGamePath);
	void processCompressedSave(const std::string& saveGamePath);
	void processAutoSave(const std::string& saveGamePath);
	void processIronManSave(const std::string& saveGamePath);
	void processSave(const std::string& saveGamePath);

	date endDate = date("1444.11.11");
	date startDate = date("1.1.1");
	GameVersion CK3Version;
	Titles titles;
	BaronyHoldings baronyHoldings;
	Characters characters;

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
		std::string gamestate;
		std::string metadata; // probably not needed
	};
	saveData saveGame;
};
} // namespace CK3

#endif // CK3_WORLD_H
