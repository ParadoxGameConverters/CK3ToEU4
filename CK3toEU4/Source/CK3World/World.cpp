#include "World.h"
#include "../Configuration/Configuration.h"
#include "../commonItems/ParserHelpers.h"
#include "Log.h"
#include <ZipFile.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

CK3::World::World(const Configuration& theConfiguration)
{
	LOG(LogLevel::Info) << "*** Hello CK3, Deus Vult! ***";

	registerKeyword("date", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString dateString(theStream);
		endDate = date(dateString.getString());
	});
	registerKeyword("bookmark_date", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString startDateString(theStream);
		startDate = date(startDateString.getString());
	});
	registerKeyword("version", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString versionString(theStream);
		CK3Version = GameVersion(versionString.getString());
		Log(LogLevel::Info) << "<> Savegame version: " << versionString.getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	Log(LogLevel::Progress) << "4 %";

	LOG(LogLevel::Info) << "-> Verifying CK3 save.";
	verifySave(theConfiguration.getSaveGamePath());
	processSave(theConfiguration.getSaveGamePath());

	auto metaData = std::istringstream(saveGame.metadata);
	parseStream(metaData);
	auto gameState = std::istringstream(saveGame.gamestate);
	parseStream(gameState);
	Log(LogLevel::Progress) << "10 %";
	clearRegisteredKeywords();


	LOG(LogLevel::Info) << "*** Good-bye CK2, rest in peace. ***";
	Log(LogLevel::Progress) << "47 %";
}

void CK3::World::processSave(const std::string& saveGamePath)
{
	switch (saveGame.saveType)
	{
		case SaveType::ZIPFILE:
			LOG(LogLevel::Info) << "-> Importing regular compressed CK3 save.";
			processCompressedSave(saveGamePath);
			break;
		case SaveType::AUTOSAVE:
			LOG(LogLevel::Info) << "-> Importing ironman CK3 autosave.";
			processAutoSave(saveGamePath);
			break;
		case SaveType::IRONMAN:
			LOG(LogLevel::Info) << "-> Importing ironman compressed CK3 save.";
			processIronManSave(saveGamePath);
			break;
		default:
			throw std::runtime_error("Unknown save type.");
	}
}


void CK3::World::verifySave(const std::string& saveGamePath)
{
	std::ifstream saveFile(fs::u8path(saveGamePath));
	if (!saveFile.is_open())
		throw std::runtime_error("Could not open save! Exiting!");

	char buffer[10];
	saveFile.get(buffer, 4);
	if (buffer[0] != 'S' || buffer[1] != 'A' || buffer[2] != 'V')
		throw std::runtime_error("Savefile of unknown type.");

	char ch;
	do
	{ // skip until newline
		ch = saveFile.get();
	} while (ch != '\n' && ch != '\r');

	saveFile.get(buffer, 10);
	if (std::string(buffer) == "meta_data")
		saveGame.saveType = SaveType::ZIPFILE;
	else
	{
		saveFile.seekg(0);
		char* bigBuf = new char[65536];
		if (saveFile.readsome(bigBuf, 65536) != 65536)
			throw std::runtime_error("Save file seems a little small.");

		for (int i = 0; i < 65533; ++i)
			if (*reinterpret_cast<uint32_t*>(bigBuf + i) == 0x04034B50 && *reinterpret_cast<uint16_t*>(bigBuf + i - 2) == 4)
			{
				saveGame.saveType = SaveType::IRONMAN;
				break;
			}
		if (saveGame.saveType != SaveType::IRONMAN)
			saveGame.saveType = SaveType::AUTOSAVE;

		delete[] bigBuf;
	}
}

void CK3::World::processCompressedSave(const std::string& saveGamePath)
{
	std::ifstream saveFile(fs::u8path(saveGamePath), std::ios::binary);
	std::stringstream inStream;
	inStream << saveFile.rdbuf();
	std::string inString = inStream.str();

	auto startMeta = inString.find_first_of("\r\n") + 1;
	auto startZipped = inString.find("PK\03\04");

	// Stripping the "Meta_data={\n" and "}\n" from the block. Let's hope they don't alter the format further.
	saveGame.metadata = inString.substr(startMeta + 12, startZipped - startMeta - 14);
	
	std::stringstream zipStream;
	zipStream << inString.substr(startZipped);

	auto zipArchive = ZipArchive::Create(zipStream);
	if (zipArchive->GetEntriesCount() != 1)
		throw std::runtime_error("Unexpected number of zipped files in archive.");

	if (zipArchive->GetEntry(0)->GetName() != "gamestate")
		throw std::runtime_error("Gamestate not found in zipped savegame.");

	saveGame.gamestate = std::string(std::istreambuf_iterator<char>(*zipArchive->GetEntry(0)->GetDecompressionStream()), {});
}

void CK3::World::processAutoSave(const std::string& saveGamePath)
{
	throw std::runtime_error("Autosaves not yet supported.");
}

void CK3::World::processIronManSave(const std::string& saveGamePath)
{
	throw std::runtime_error("Ironman saves not yet supported.");
}
