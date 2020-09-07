#include "World.h"
#include "../Configuration/Configuration.h"
#include "../commonItems/ParserHelpers.h"
#include "../Helpers/rakaly_wrapper.h"
#include "Log.h"
#include <ZipFile.h>
#include <filesystem>
#include <fstream>
#include "OSCompatibilityLayer.h"

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
	registerKeyword("landed_titles", [this](const std::string& unused, std::istream& theStream) {
		Log(LogLevel::Info) << "-> Loading landed titles.";
		titles = Titles(theStream);
		Log(LogLevel::Info) << "<> Loaded " << titles.getTitles().size() << " landed titles.";
	});
	registerKeyword("provinces", [this](const std::string& unused, std::istream& theStream) {
		Log(LogLevel::Info) << "-> Loading barony holdings.";
		baronyHoldings = BaronyHoldings(theStream);
		Log(LogLevel::Info) << "<> Loaded " << baronyHoldings.getBaronyHoldings().size() << " baronies.";
	});
	registerKeyword("living", [this](const std::string& unused, std::istream& theStream) {
		Log(LogLevel::Info) << "-> Loading alive human beings.";
		characters = Characters(theStream);
		Log(LogLevel::Info) << "<> Loaded " << characters.getCharacters().size() << " human entities.";
	});
	registerKeyword("dynasties", [this](const std::string& unused, std::istream& theStream) {
		Log(LogLevel::Info) << "-> Loading dynasties.";
		dynasties = Dynasties(theStream);
		houses = dynasties.getHouses(); // Do not access houses in dynasties after this - there are none and will crash.
		Log(LogLevel::Info) << "<> Loaded " << dynasties.getDynasties().size() << " dynasties and " << houses.getHouses().size() << " houses.";
	});
	registerKeyword("religion", [this](const std::string& unused, std::istream& theStream) {
		Log(LogLevel::Info) << "-> Loading religions.";
		religions = Religions(theStream);
		faiths = religions.getFaiths(); // Do not access faiths in religions after this - there are none and will crash.
		Log(LogLevel::Info) << "<> Loaded " << religions.getReligions().size() << " religions and " << faiths.getFaiths().size() << " faiths.";
	});
	registerKeyword("coat_of_arms", [this](const std::string& unused, std::istream& theStream) {
		Log(LogLevel::Info) << "-> Loading garments of limbs.";
		coats = CoatsOfArms(theStream);
		Log(LogLevel::Info) << "<> Loaded " << coats.getCoats().size() << " wearables.";
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	Log(LogLevel::Progress) << "4 %";

	LOG(LogLevel::Info) << "-> Verifying CK3 save.";
	verifySave(theConfiguration.getSaveGamePath());
	processSave(theConfiguration.getSaveGamePath());

	auto metaData = std::istringstream(saveGame.metadata);
	parseStream(metaData);

	primeLaFabricaDeColor(theConfiguration);
	
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
		case SaveType::INVALID:
			throw std::runtime_error("Unknown save type.");
	}
}


void CK3::World::verifySave(const std::string& saveGamePath)
{
	std::ifstream saveFile(fs::u8path(saveGamePath), std::ios::binary);
	if (!saveFile.is_open())
		throw std::runtime_error("Could not open save! Exiting!");

	char buffer[10];
	saveFile.get(buffer, 4);
	if (buffer[0] != 'S' || buffer[1] != 'A' || buffer[2] != 'V')
		throw std::runtime_error("Savefile of unknown type.");

	char ch;
	do
	{ // skip until newline
		ch = static_cast<char>(saveFile.get());
	} while (ch != '\n' && ch != '\r');

	saveFile.get(buffer, 10);
	if (std::string(buffer) == "meta_data")
		saveGame.saveType = SaveType::ZIPFILE;
	else
	{
		saveFile.seekg(0, std::ios::end);
		const auto length = saveFile.tellg();
		if (length < 65536)
		{
			throw std::runtime_error("Savegame seems a bit too small.");
		}
		saveFile.seekg(0, std::ios::beg);
		char* bigBuf = new char[65536];
		saveFile.read(bigBuf, 65536);
		if (saveFile.gcount() < 65536)
			throw std::runtime_error("Read only: " + std::to_string(saveFile.gcount()));
		for (int i = 0; i < 65533; ++i)
			if (*reinterpret_cast<uint32_t*>(bigBuf + i) == 0x04034B50 && *reinterpret_cast<uint16_t*>(bigBuf + i - 2) == 4)
			{
				saveGame.zipStart = i;
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

	// Stripping the "meta_data={\n" and "}\n" from the block. Let's hope they don't alter the format further.
	saveGame.metadata = inString.substr(startMeta + 12, startZipped - startMeta - 14);

	std::stringstream zipStream;
	zipStream << inString.substr(startZipped);

	auto zipArchive = ZipArchive::Create(zipStream);
	if (zipArchive->GetEntriesCount() != 1)
		throw std::runtime_error("Unexpected number of zipped files in the savegame.");

	if (zipArchive->GetEntry(0)->GetName() != "gamestate")
		throw std::runtime_error("Gamestate file not found in zipped savegame.");

	saveGame.gamestate = std::string(std::istreambuf_iterator<char>(*zipArchive->GetEntry(0)->GetDecompressionStream()), {});
}

void CK3::World::processAutoSave(const std::string& saveGamePath)
{
	std::ifstream saveFile(fs::u8path(saveGamePath), std::ios::binary);
	std::stringstream inStream;
	inStream << saveFile.rdbuf();
	std::string inBinary(std::istreambuf_iterator<char>(inStream), {});
	saveGame.gamestate = rakaly::meltCK3(inBinary);

	auto startMeta = saveGame.gamestate.find_first_of("\r\n");
	auto endFile = saveGame.gamestate.size();
	saveGame.gamestate = saveGame.gamestate.substr(startMeta, endFile - startMeta);

	auto endMeta = saveGame.gamestate.find("ironman=no");
	// Again, strip the "meta_data={\n" and the "}\n"
	saveGame.metadata = saveGame.gamestate.substr(12, endMeta - 1);
}

void CK3::World::processIronManSave(const std::string& saveGamePath)
{
	std::ifstream saveFile(fs::u8path(saveGamePath), std::ios::binary);
	std::stringstream inStream;
	inStream << saveFile.rdbuf();
	std::string inBinary(std::istreambuf_iterator<char>(inStream), {});

	std::string meta = rakaly::meltCK3(inBinary.substr(0, saveGame.zipStart));
	auto startMeta = meta.find_first_of("\r\n");
	auto endMeta = meta.find_last_of("}");
	// Again, strip the "meta_data={\n" and the "}\n"
	saveGame.metadata = meta.substr(startMeta + 12, endMeta - startMeta - 12);

	saveGame.gamestate = rakaly::meltCK3(inBinary);
	auto skipLine = saveGame.gamestate.find_first_of("\r\n");
	auto endFile = saveGame.gamestate.size();
	saveGame.gamestate = saveGame.gamestate.substr(skipLine, endFile - skipLine);
}

void CK3::World::primeLaFabricaDeColor(const Configuration& theConfiguration)
{
	Log(LogLevel::Info) << "-> Loading colors.";
	for (const auto& file: Utils::GetAllFilesInFolder(theConfiguration.getCK3Path() + "/game/common/named_colors"))
		namedColors.loadColors(theConfiguration.getCK3Path() + "/game/common/named_colors/" + file);
	Log(LogLevel::Info) << "<> Loaded " << laFabricaDeColor.getRegisteredColors().size() << " colors.";
}
