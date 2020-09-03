#include "World.h"
#include "Log.h"
#include <ZipFile.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

CK3::World::World(const Configuration& theConfiguration)
{
	LOG(LogLevel::Info) << "*** Hello CK3, Deus Vult! ***";

	LOG(LogLevel::Info) << "-> Verifying CK3 save.";
	const std::string saveGamePath("input.ck3"); // TODO: read filepath from Configuration
	verifySave(saveGamePath);

	switch (saveGame.saveType)
	{
		case SaveType::ZIPFILE:
			LOG(LogLevel::Info) << "-> Importing compressed CK3 save.";
			processCompressedSave(saveGamePath);
			break;
		case SaveType::AUTOSAVE:
			LOG(LogLevel::Info) << "-> Importing CK3 autosave.";
			processAutoSave(saveGamePath);
			break;
		case SaveType::IRONMAN:
			LOG(LogLevel::Info) << "-> Importing ironman CK3 save.";
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
	do { // skip until newline
		ch = saveFile.get();
	} while (ch != '\n' && ch != '\r');

	saveFile.get(buffer, 10);
	if (std::string(buffer) == "meta_data")
		saveGame.saveType = SaveType::ZIPFILE;
	else
	{
		saveFile.seekg(0);
		char * bigBuf = new char[65536];
		saveFile.read(bigBuf, 65536);
		if (saveFile.gcount() != 65536)
			throw std::runtime_error("Save file seems a little small.");

		for (int i=0; i<65533; ++i)
			if (*reinterpret_cast<uint32_t*>(bigBuf+i) == 0x04034B50 && *reinterpret_cast<uint16_t*>(bigBuf+i-2) == 4)
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

	int startMeta = inString.find_first_of("\r\n") + 1;
	int startZipped = inString.find("PK\03\04");

	saveGame.metadata = inString.substr(startMeta, startZipped-startMeta);

	std::stringstream zipStream;
	zipStream << inString.substr(startZipped);

	auto zipArchive = ZipArchive::Create(zipStream);
	if (zipArchive->GetEntriesCount() != 1)
		throw std::runtime_error("Unexpected number of zipped files in archive.");

	if (zipArchive->GetEntry(0)->GetName() != "gamestate")
		throw std::runtime_error("gamestate not found in zipped archive.");

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

