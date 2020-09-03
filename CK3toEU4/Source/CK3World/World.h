#ifndef CK3_WORLD_H
#define CK3_WORLD_H

#include "Parser.h"

class Configuration;

namespace CK3
{
class World: commonItems::parser
{
public:
	explicit World(const Configuration& theConfiguration);
private:
	void verifySave(const std::string& saveGamePath);
	void processCompressedSave(const std::string& saveGamePath);
	void processAutoSave(const std::string& saveGamePath);
	void processIronManSave(const std::string& saveGamePath);

	enum class SaveType
	{
		INVALID=0,
		ZIPFILE,
		AUTOSAVE,
		IRONMAN
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

