#include "World.h"
#include "../Configuration/Configuration.h"
#include "../Helpers/rakaly_wrapper.h"
#include "../commonItems/ParserHelpers.h"
#include "Characters/Character.h"
#include "Characters/CharacterDomain.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "Titles/Title.h"
#include <ZipFile.h>
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

CK3::World::World(const std::shared_ptr<Configuration>& theConfiguration)
{
	LOG(LogLevel::Info) << "*** Hello CK3, Deus Vult! ***";

	registerKeyword("mods", [theConfiguration](const std::string& unused, std::istream& theStream) {
		Log(LogLevel::Info) << "-> Detecting used mods.";
		const auto modsList = commonItems::stringList(theStream).getStrings();
		theConfiguration->setModFileNames(std::set(modsList.begin(), modsList.end()));
		Log(LogLevel::Info) << "<> Savegame claims " << theConfiguration->getModFileNames().size() << " mods used.";
	});
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
	registerKeyword("variables", [this](const std::string& unused, std::istream& theStream) {
		Log(LogLevel::Info) << "-> Loading variable flags.";
		flags = Flags(theStream);
		Log(LogLevel::Info) << "<> Loaded " << flags.getFlags().size() << " variable flags.";
	});
	registerKeyword("landed_titles", [this](const std::string& unused, std::istream& theStream) {
		Log(LogLevel::Info) << "-> Loading titles.";
		titles = Titles(theStream);
		const auto& counter = titles.getCounter();
		Log(LogLevel::Info) << "<> Loaded " << titles.getTitles().size() << " titles: " << counter[0] << "b " << counter[1] << "c " << counter[2] << "d "
								  << counter[3] << "k " << counter[4] << "e.";
	});
	registerKeyword("provinces", [this](const std::string& unused, std::istream& theStream) {
		Log(LogLevel::Info) << "-> Loading provinces.";
		provinceHoldings = ProvinceHoldings(theStream);
		Log(LogLevel::Info) << "<> Loaded " << provinceHoldings.getProvinceHoldings().size() << " provinces.";
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
	registerKeyword("county_manager", [this](const std::string& unused, std::istream& theStream) {
		Log(LogLevel::Info) << "-> Loading county details.";
		countyDetails = CountyDetails(theStream);
		Log(LogLevel::Info) << "<> Loaded " << countyDetails.getCountyDetails().size() << " county details.";
	});
	registerKeyword("culture_manager", [this](const std::string& unused, std::istream& theStream) {
		Log(LogLevel::Info) << "-> Loading cultures.";
		cultures = Cultures(theStream);
		Log(LogLevel::Info) << "<> Loaded " << cultures.getCultures().size() << " cultures.";
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	Log(LogLevel::Progress) << "4 %";

	LOG(LogLevel::Info) << "-> Verifying CK3 save.";
	verifySave(theConfiguration->getSaveGamePath());
	processSave(theConfiguration->getSaveGamePath());
	Log(LogLevel::Progress) << "5 %";

	auto metaData = std::istringstream(saveGame.metadata);
	parseStream(metaData);
	Log(LogLevel::Progress) << "10 %";

	LOG(LogLevel::Info) << "* Priming Converter Components *";
	mods.loadModDirectory(*theConfiguration);
	primeLaFabricaDeColor(*theConfiguration);
	loadLandedTitles(*theConfiguration);
	Log(LogLevel::Progress) << "15 %";

	LOG(LogLevel::Info) << "* Parsing Gamestate *";
	auto gameState = std::istringstream(saveGame.gamestate);
	parseStream(gameState);
	Log(LogLevel::Progress) << "20 %";
	clearRegisteredKeywords();

	LOG(LogLevel::Info) << "* Gamestate Parsing Complete, Weaving Internals *";
	crosslinkDatabases();
	Log(LogLevel::Progress) << "30 %";

	// processing
	LOG(LogLevel::Info) << "-- Flagging HRE Provinces";
	flagHREProvinces(*theConfiguration);
	LOG(LogLevel::Info) << "-- Shattering HRE";
	shatterHRE(*theConfiguration);
	LOG(LogLevel::Info) << "-- Shattering Empires";
	shatterEmpires(*theConfiguration);

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
	for (const auto& file: Utils::GetAllFilesInFolder(theConfiguration.getCK3Path() + "common/named_colors"))
	{
		if (file.find(".txt") == std::string::npos)
			continue;
		namedColors.loadColors(theConfiguration.getCK3Path() + "common/named_colors/" + file);
	}
	for (const auto& mod: mods.getMods())
	{
		if (!Utils::DoesFolderExist(mod.second + "common/named_colors"))
			continue;
		Log(LogLevel::Info) << "<> Loading some colors from " << mod.first;
		for (const auto& file: Utils::GetAllFilesInFolder(mod.second + "common/named_colors"))
		{
			if (file.find(".txt") == std::string::npos)
				continue;
			namedColors.loadColors(mod.second + "common/named_colors/" + file);
		}
	}
	Log(LogLevel::Info) << "<> Loaded " << laFabricaDeColor.getRegisteredColors().size() << " colors.";
}


void CK3::World::loadLandedTitles(const Configuration& theConfiguration)
{
	Log(LogLevel::Info) << "-> Loading Landed Titles.";
	for (const auto& file: Utils::GetAllFilesInFolder(theConfiguration.getCK3Path() + "common/landed_titles"))
	{
		if (file.find(".txt") == std::string::npos)
			continue;
		landedTitles.loadTitles(theConfiguration.getCK3Path() + "common/landed_titles/" + file);
	}
	for (const auto& mod: mods.getMods())
	{
		if (!Utils::DoesFolderExist(mod.second + "common/landed_titles"))
			continue;
		Log(LogLevel::Info) << "<> Loading some landed titles from " << mod.first;
		for (const auto& file: Utils::GetAllFilesInFolder(mod.second + "common/landed_titles"))
		{
			if (file.find(".txt") == std::string::npos)
				continue;
			landedTitles.loadTitles(mod.second + "common/landed_titles/" + file);
		}
	}
	Log(LogLevel::Info) << "<> Loaded " << landedTitles.getFoundTitles().size() << " landed titles.";
}

void CK3::World::crosslinkDatabases()
{
	Log(LogLevel::Info) << "-> Loading Cultures into Counties.";
	countyDetails.linkCultures(cultures);
	Log(LogLevel::Info) << "-> Loading Cultures into Characters.";
	characters.linkCultures(cultures);
	Log(LogLevel::Info) << "-> Loading Faiths into Counties.";
	countyDetails.linkFaiths(faiths);
	Log(LogLevel::Info) << "-> Loading Faiths into Characters.";
	characters.linkFaiths(faiths);
	Log(LogLevel::Info) << "-> Loading Faiths into Religions.";
	religions.linkFaiths(faiths);
	Log(LogLevel::Info) << "-> Loading Religions into Faiths.";
	faiths.linkReligions(religions);
	Log(LogLevel::Info) << "-> Loading Coats into Coats.";
	coats.linkParents(titles);
	Log(LogLevel::Info) << "-> Loading Coats into Dynasties.";
	dynasties.linkCoats(coats);
	Log(LogLevel::Info) << "-> Loading Coats into Titles.";
	titles.linkCoats(coats);
	Log(LogLevel::Info) << "-> Loading Holdings into Clay.";
	landedTitles.linkProvinceHoldings(provinceHoldings);
	Log(LogLevel::Info) << "-> Loading Counties into Clay.";
	landedTitles.linkCountyDetails(countyDetails);
	Log(LogLevel::Info) << "-> Loading Dynasties into Houses.";
	houses.linkDynasties(dynasties);
	Log(LogLevel::Info) << "-> Loading Houses into Characters.";
	characters.linkHouses(houses);
	Log(LogLevel::Info) << "-> Loading Titles into Characters.";
	characters.linkTitles(titles);
	Log(LogLevel::Info) << "-> Loading Titles into Titles.";
	titles.linkTitles();
	Log(LogLevel::Info) << "-> Loading Titles into Clay.";
	landedTitles.linkTitles(titles);
	Log(LogLevel::Info) << "-> Loading Characters into Characters.";
	characters.linkCharacters();
	Log(LogLevel::Info) << "-> Loading Characters into Titles.";
	titles.linkCharacters(characters);
	Log(LogLevel::Info) << "-> Loading Clay into Titles.";
	titles.linkLandedTitles(landedTitles);
}

void CK3::World::flagHREProvinces(const Configuration& theConfiguration) const
{
	std::string hreTitle;
	switch (theConfiguration.getHRE())
	{
		case Configuration::I_AM_HRE::HRE:
			hreTitle = "e_hre";
			break;
		case Configuration::I_AM_HRE::BYZANTIUM:
			hreTitle = "e_byzantium";
			break;
		case Configuration::I_AM_HRE::ROME:
			hreTitle = "e_roman_empire";
			break;
		case Configuration::I_AM_HRE::CUSTOM:
			hreTitle = iAmHreMapper.getHRE();
			break;
		case Configuration::I_AM_HRE::NONE:
			Log(LogLevel::Info) << ">< HRE Provinces not available due to configuration disabling HRE Mechanics.";
			return;
	}
	const auto& allTitles = titles.getTitles();
	const auto& theHre = allTitles.find(hreTitle);
	if (theHre == allTitles.end())
	{
		Log(LogLevel::Info) << ">< HRE Provinces not available, " << hreTitle << " not found!";
		return;
	}
	if (theHre->second->getDFVassals().empty())
	{
		Log(LogLevel::Info) << ">< HRE Provinces not available, " << hreTitle << " has no vassals!";
		return;
	}

	const auto counter = theHre->second->flagDeJureHREProvinces();
	Log(LogLevel::Info) << "<> " << counter << " HRE provinces flagged.";
}

void CK3::World::shatterHRE(const Configuration& theConfiguration) const
{
	std::string hreTitle;
	switch (theConfiguration.getHRE())
	{
		case Configuration::I_AM_HRE::HRE:
			hreTitle = "e_hre";
			break;
		case Configuration::I_AM_HRE::BYZANTIUM:
			hreTitle = "e_byzantium";
			break;
		case Configuration::I_AM_HRE::ROME:
			hreTitle = "e_roman_empire";
			break;
		case Configuration::I_AM_HRE::CUSTOM:
			hreTitle = iAmHreMapper.getHRE();
			break;
		case Configuration::I_AM_HRE::NONE:
			Log(LogLevel::Info) << ">< HRE Mechanics and shattering overridden by configuration.";
			return;
	}
	const auto& allTitles = titles.getTitles();
	const auto& theHre = allTitles.find(hreTitle);
	if (theHre == allTitles.end())
	{
		Log(LogLevel::Info) << ">< HRE shattering cancelled, " << hreTitle << " not found!";
		return;
	}
	if (theHre->second->getDFVassals().empty())
	{
		Log(LogLevel::Info) << ">< HRE shattering cancelled, " << hreTitle << " has no vassals!";
		return;
	}
	if (!theHre->second->getHolder())
	{
		Log(LogLevel::Info) << ">< HRE shattering cancelled, " << hreTitle << " has no holder!";
		return;
	}
	const auto& hreHolder = theHre->second->getHolder();
	bool emperorSet = false; // "Emperor", in this context, is not a person but the resulting primary duchy/kingdom title of said person.

	// First we are composing a list of all HRE members. These are duchies,
	// so we're also ripping them from under any potential kingdoms.
	std::map<int, std::shared_ptr<Title>> hreMembers;

	for (const auto& vassal: theHre->second->getDFVassals())
	{
		if (vassal.second->getName().find("d_") == 0 || vassal.second->getName().find("c_") == 0)
		{
			hreMembers.insert(vassal);
		}
		else if (vassal.second->getName().find("k_") == 0)
		{
			if (vassal.second->getName() == "k_papal_state" || vassal.second->getName() == "k_orthodox" ||
				 theConfiguration.getShatterHRELevel() == Configuration::SHATTER_HRE_LEVEL::KINGDOM) // hard override for special HRE members
			{
				hreMembers.insert(vassal);
			}
			else
			{
				for (const auto& vassalvassal: vassal.second->getDFVassals())
				{
					hreMembers.insert(vassalvassal);
				}
				// Bricking the kingdom.
				vassal.second->brickTitle();
			}
		}
		else if (vassal.second->getName().find("b_") != 0)
		{
			Log(LogLevel::Warning) << "Unrecognized HRE vassal: " << vassal.first << " - " << vassal.second->getName();
		}
	}

	// Locating HRE emperor. Unlike CK2, we'll using first non-hreTitle non-landless title from hreHolder's domain.
	for (const auto& hreHolderTitle: hreHolder->second->getDomain()->getDomain())
	{
		if (hreHolderTitle.second->getName() == hreTitle) // this is what we're breaking, ignore it.
			continue;
		if (hreHolderTitle.second->getName().find("b_") == 0) // Absolutely ignore baronies.
			continue;
		if (hreHolderTitle.second->getClay() && !hreHolderTitle.second->getClay()->isLandless())
		{ // looks solid.
			hreHolderTitle.second->setHREEmperor();
			Log(LogLevel::Debug) << "Flagging " << hreHolderTitle.second->getName() << " as His HREship.";
			emperorSet = true;
			break;
		}
	}

	if (!emperorSet)
		Log(LogLevel::Warning) << "Couldn't flag His HREship as emperor does not own any viable titles!";

	// We're flagging hre members as such, as well as setting them free.
	for (const auto& member: hreMembers)
	{
		member.second->setInHRE();
		member.second->grantIndependence();
	}

	// Finally we brick the hre.
	theHre->second->brickTitle();
	Log(LogLevel::Info) << "<> " << hreMembers.size() << " HRE members released.";
}

void CK3::World::shatterEmpires(const Configuration& theConfiguration) const
{
	if (theConfiguration.getShatterEmpires() == Configuration::SHATTER_EMPIRES::NONE)
	{
		Log(LogLevel::Info) << ">< Empire shattering disabled by configuration.";
		return;
	}

	bool shatterKingdoms = true; // the default.
	switch (theConfiguration.getShatterLevel())
	{
		case Configuration::SHATTER_LEVEL::KINGDOM:
			shatterKingdoms = false;
			break;
		case Configuration::SHATTER_LEVEL::DUTCHY:
			shatterKingdoms = true;
			break;
	}
	const auto& allTitles = titles.getTitles();

	for (const auto& empire: allTitles)
	{
		if (theConfiguration.getShatterEmpires() == Configuration::SHATTER_EMPIRES::CUSTOM && !shatterEmpiresMapper.isEmpireShatterable(empire.first))
			continue; // Only considering those listed.
		if (empire.first.find("e_") != 0 && theConfiguration.getShatterEmpires() != Configuration::SHATTER_EMPIRES::CUSTOM)
			continue; // Otherwise only empires.
		if (empire.second->getDFVassals().empty())
			continue; // Not relevant.

		// First we are composing a list of all members.
		std::map<int, std::shared_ptr<Title>> members;
		for (const auto& vassal: empire.second->getDFVassals())
		{
			if (vassal.second->getName().find("d_") == 0 || vassal.second->getName().find("c_") == 0)
			{
				members.insert(vassal);
			}
			else if (vassal.second->getName().find("k_") == 0)
			{
				if (shatterKingdoms && vassal.second->getName() != "k_papal_state" && vassal.second->getName() != "k_orthodox")
				{ // hard override for special empire members
					for (const auto& vassalvassal: vassal.second->getDFVassals())
					{
						members.insert(vassalvassal);
					}
					// Bricking the kingdom
					vassal.second->brickTitle();
				}
				else
				{
					// Not shattering kingdoms.
					members.insert(vassal);
				}
			}
			else if (vassal.second->getName().find("b_") != 0)
			{
				Log(LogLevel::Warning) << "Unrecognized vassal level: " << vassal.first;
			}
		}

		// grant independence to ex-vassals.
		for (const auto& member: members)
		{
			member.second->grantIndependence();
		}

		// Finally, dispose of the shell.
		empire.second->brickTitle();
		Log(LogLevel::Info) << "<> " << empire.first << " shattered, " << members.size() << " members released.";
	}
}
