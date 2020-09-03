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
	verifySave("input.ck3"); // TODO: read filepath from Configuration

	if (saveGame.compressed)
	{
		LOG(LogLevel::Info) << "-> Importing compressed CK3 save.";
		processCompressedSave("input.ck3"); // TODO: idem
	}
	else
	{
		LOG(LogLevel::Info) << "-> Importing uncompressed CK3 save.";
		processUncompressedSave("input.ck3"); // TODO: ditto
	}
}

void CK3::World::verifySave(const std::string& saveGamePath)
{
	std::ifstream saveFile(fs::u8path(saveGamePath));
	if (!saveFile.is_open())
		throw std::runtime_error("Could not open save! Exiting!");

	char buffer[4];
	saveFile.get(buffer, 4);
	if (buffer[0] == 'S' && buffer[1] == 'A' && buffer[2] == 'V')
		saveGame.compressed = true;

	saveFile.close();
}

void CK3::World::processUncompressedSave(const std::string& saveGamePath)
{
	std::ifstream saveFile(fs::u8path(saveGamePath), std::ios::binary);
	std::stringstream inStream;
	inStream << saveFile.rdbuf();
	saveGame.gamestate = inStream.str();
	saveFile.close();
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

