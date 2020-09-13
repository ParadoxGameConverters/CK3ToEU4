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
	loadCharacterTraits(*theConfiguration);
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
	LOG(LogLevel::Info) << "-- Filtering Independent Titles";
	filterIndependentTitles();
	LOG(LogLevel::Info) << "-- Splitting Off Vassals";
	splitVassals(*theConfiguration);
	LOG(LogLevel::Info) << "-- Rounding Up Some People";
	gatherCourtierNames();
	LOG(LogLevel::Info) << "-- Congregating DeFacto Counties for Independent Titles";
	congregateDFCounties();
	LOG(LogLevel::Info) << "-- Congregating DeJure Counties for Independent Titles";
	congregateDJCounties();
	LOG(LogLevel::Info) << "-- Filtering Landless Titles";
	filterLandlessTitles();
	LOG(LogLevel::Info) << "-- Distributing Electorates";
	setElectors();

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

void CK3::World::loadCharacterTraits(const Configuration& theConfiguration)
{
	LOG(LogLevel::Info) << "-> Examiming Personalities";
	for (const auto& file: Utils::GetAllFilesInFolder(theConfiguration.getCK3Path() + "common/traits"))
	{
		if (file.find(".txt") == std::string::npos)
			continue;
		traitScraper.loadTraits(theConfiguration.getCK3Path() + "common/traits/" + file);
	}
	for (const auto& mod: mods.getMods())
	{
		if (!Utils::DoesFolderExist(mod.second + "common/traits"))
			continue;
		Log(LogLevel::Info) << "<> Loading some character traits from " << mod.first;
		for (const auto& file: Utils::GetAllFilesInFolder(mod.second + "common/traits"))
		{
			if (file.find(".txt") == std::string::npos)
				continue;
			traitScraper.loadTraits(mod.second + "common/traits/" + file);
		}
	}
	LOG(LogLevel::Info) << ">> " << traitScraper.getTraits().size() << " personalities scrutinized.";
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
	Log(LogLevel::Info) << "-> Loading Traits into Characters.";
	characters.linkTraits(traitScraper);
}

void CK3::World::flagHREProvinces(const Configuration& theConfiguration)
{
	std::string hreTitleStr;
	switch (theConfiguration.getHRE())
	{
		case Configuration::I_AM_HRE::HRE:
			hreTitleStr = "e_hre";
			break;
		case Configuration::I_AM_HRE::BYZANTIUM:
			hreTitleStr = "e_byzantium";
			break;
		case Configuration::I_AM_HRE::ROME:
			hreTitleStr = "e_roman_empire";
			break;
		case Configuration::I_AM_HRE::CUSTOM:
			hreTitleStr = iAmHreMapper.getHRE();
			break;
		case Configuration::I_AM_HRE::NONE:
			Log(LogLevel::Info) << ">< HRE Provinces not available due to configuration disabling HRE Mechanics.";
			return;
	}
	const auto& allTitles = titles.getTitles();
	const auto& theHre = allTitles.find(hreTitleStr);
	if (theHre == allTitles.end())
	{
		Log(LogLevel::Info) << ">< HRE Provinces not available, " << hreTitleStr << " not found!";
		return;
	}
	if (theHre->second->getDFVassals().empty())
	{
		Log(LogLevel::Info) << ">< HRE Provinces not available, " << hreTitleStr << " has no vassals!";
		return;
	}
	if (!theHre->second->getHolder())
	{
		Log(LogLevel::Info) << ">< HRE Provinces not available, " << hreTitleStr << " has no holder!";
		return;
	}

	// store for later.
	hreTitle = std::make_pair(hreTitleStr, theHre->second);

	const auto counter = theHre->second->flagDeJureHREProvinces();
	Log(LogLevel::Info) << "<> " << counter << " HRE provinces flagged.";
}

void CK3::World::shatterHRE(const Configuration& theConfiguration) const
{
	if (!hreTitle)
		return;
	const auto& hreHolder = hreTitle->second->getHolder();
	bool emperorSet = false; // "Emperor", in this context, is not a person but the resulting primary duchy/kingdom title of said person.

	// First we are composing a list of all HRE members. These are duchies,
	// so we're also ripping them from under any potential kingdoms.
	std::map<int, std::shared_ptr<Title>> hreMembers;

	for (const auto& vassal: hreTitle->second->getDFVassals())
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
		if (hreHolderTitle.second->getName() == hreTitle->first) // this is what we're breaking, ignore it.
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
	hreTitle->second->brickTitle();
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
		if (hreTitle && empire.first == hreTitle->first)
			continue; // This is HRE, wrong function for that one.
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

void CK3::World::filterIndependentTitles()
{
	const auto& allTitles = titles.getTitles();
	std::map<std::string, std::shared_ptr<Title>> potentialIndeps;

	for (const auto& title: allTitles)
	{
		if (!title.second->getHolder())
			continue; // don't bother with titles without holders.
		if (!title.second->getDFLiege())
		{
			// this is a potential indep.
			potentialIndeps.insert(title);
		}
	}

	// Check if the holder holds any actual land (b|c_something). (Only necessary for the holder,
	// no need to recurse, we're just filtering landless titular titles like mercenaries
	// or landless Pope. If a character holds a landless titular title along actual title
	// (like Caliphate), it's not relevant at this stage as he's independent anyway.

	// First, split off all county_title holders into a container.
	std::set<int> countyHolders;
	std::map<int, std::map<std::string, std::shared_ptr<Title>>> allTitleHolders;
	for (const auto& title: allTitles)
	{
		if (title.second->getHolder() && title.second->getName().find("c_") == 0)
			countyHolders.insert(title.second->getHolder()->first);
		allTitleHolders[title.second->getHolder()->first].insert(title);
	}

	// Then look at all potential indeps and see if their holders hold physical clay.
	auto counter = 0;
	for (const auto& indep: potentialIndeps)
	{
		const auto& holderID = indep.second->getHolder()->first;
		if (countyHolders.count(holderID))
		{
			// this fellow holds a county, so his indep title is an actual title.
			independentTitles.insert(indep);
			counter++;
			// Set The Pope
			if (indep.first == "k_papal_state")
			{
				indep.second->setThePope();
				Log(LogLevel::Debug) << indep.first << " is the Pope.";
			}
			else
			{
				if (allTitleHolders[holderID].count("k_papal_state"))
				{
					indep.second->setThePope();
					Log(LogLevel::Debug) << indep.first << " belongs to the Pope.";
				}
			}
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " independent titles recognized.";
}

void CK3::World::splitVassals(const Configuration& theConfiguration)
{
	if (theConfiguration.getSplitVassals() == Configuration::SPLITVASSALS::NO)
	{
		Log(LogLevel::Info) << ">< Splitting vassals disabled by configuration.";
		return;
	}

	std::map<std::string, std::shared_ptr<Title>> newIndeps;

	// We know who's independent. We can go through all indeps and see what should be an independent vassal.
	for (const auto& title: independentTitles)
	{
		if (title.second->isThePope())
			continue; // Not touching the pope.
		// let's not split hordes or tribals. <- TODO: Add horde here once some DLC drops.
		if (title.second->getHolder()->second->getDomain()->getGovernment() == "tribal_government")
			continue;
		auto relevantVassals = 0;
		std::string relevantVassalPrefix;
		if (title.first.find("e_") == 0)
			relevantVassalPrefix = "k_";
		else if (title.first.find("k_") == 0)
			relevantVassalPrefix = "d_";
		else
			continue; // Not splitting off counties.
		for (const auto& vassal: title.second->getDFVassals())
		{
			if (vassal.second->getName().find(relevantVassalPrefix) != 0)
				continue; // they are not relevant
			if (vassal.second->coalesceDFCounties().empty())
				continue; // no land, not relevant
			relevantVassals++;
		}
		if (!relevantVassals)
			continue;																		// no need to split off anything.
		const auto& countiesClaimed = title.second->coalesceDFCounties(); // this is our primary total.
		for (const auto& vassal: title.second->getDFVassals())
		{
			if (vassal.second->getName().find(relevantVassalPrefix) != 0)
				continue; // they are not relevant
			if (vassal.second->getHolder()->first == title.second->getHolder()->first)
				continue; // Not splitting our own land.
			const auto& vassalProvincesClaimed = vassal.second->coalesceDFCounties();

			// a vassal goes indep if they control 1/relevantvassals + 10% land.
			const double threshold = static_cast<double>(countiesClaimed.size()) / relevantVassals + 0.1 * static_cast<double>(countiesClaimed.size());
			if (static_cast<double>(vassalProvincesClaimed.size()) > threshold)
				newIndeps.insert(std::pair(vassal.second->getName(), vassal.second));
		}
	}

	// Now let's free them.
	for (const auto& newIndep: newIndeps)
	{
		const auto& liege = newIndep.second->getDFLiege();
		liege->second->addGeneratedVassal(newIndep);
		newIndep.second->loadGeneratedLiege(std::pair(liege->second->getName(), liege->second));
		newIndep.second->grantIndependence();
		independentTitles.insert(newIndep);
	}
	Log(LogLevel::Info) << "<> " << newIndeps.size() << " vassals liberated from immediate integration.";
}

void CK3::World::gatherCourtierNames()
{
	// We're using this function to locate courtiers, assemble their names as potential Monarch Names in EU4,
	// and also while at it, to see if they hold adviser jobs. It's anything but trivial, as being employed doesn't equate with
	// being a councilor, nor do landed councilors have employers if they work for their liege.

	auto counter = 0;
	auto counterAdvisors = 0;
	std::map<int, std::map<std::string, bool>> holderCourtiers;						// holder-name/male
	std::map<int, std::map<int, std::shared_ptr<Character>>> holderCouncilors; // holder-councilors

	for (const auto& character: characters.getCharacters())
	{
		// Hello. Are you an employed individual?
		if (!character.second->isCouncilor() && !character.second->getEmployer())
			continue;
		// If you have a steady job, we need your employer's references.
		if (character.second->isCouncilor())
		{
			if (character.second->getEmployer())
			{
				// easiest case.
				holderCourtiers[character.second->getEmployer()->first].insert(std::pair(character.second->getName(), !character.second->isFemale()));
				holderCouncilors[character.second->getEmployer()->first].insert(character);
			}
			else if (character.second->getDomain() && !character.second->getDomain()->getDomain().empty())
			{
				// this councilor is landed and works for his liege.
				const auto& characterPrimaryTitle = character.second->getDomain()->getDomain()[0];
				const auto& liegeTitle = characterPrimaryTitle.second->getDFLiege();
				if (!liegeTitle)
					continue; // I dislike this character. I think it is time he was let go.
				const auto& liege = liegeTitle->second->getHolder();
				if (!liege)
					continue; // Or maybe we should fire his liege.
				holderCourtiers[liege->first].insert(std::pair(character.second->getName(), character.second->isFemale()));
				holderCouncilors[liege->first].insert(character);
			}
			else
			{
				// Doesn't have employer and doesn't have land but is councilor. Bollocks.
				continue;
			}
		}
		else if (character.second->getEmployer())
		{
			// Being employed but without a council task means a knight or physician or similar. Works for us.
			holderCourtiers[character.second->getEmployer()->first].insert(std::pair(character.second->getName(), !character.second->isFemale()));
		}
	}

	// We're only interested in those working for indeps.
	for (const auto& title: independentTitles)
	{
		const auto containerItr = holderCourtiers.find(title.second->getHolder()->first);
		if (containerItr != holderCourtiers.end())
		{
			title.second->getHolder()->second->loadCourtierNames(containerItr->second);
			counter += static_cast<int>(containerItr->second.size());
		}
		const auto councilorItr = holderCouncilors.find(title.second->getHolder()->first);
		if (councilorItr != holderCouncilors.end())
		{
			title.second->getHolder()->second->loadCouncilors(councilorItr->second);
			counterAdvisors += static_cast<int>(councilorItr->second.size());
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " people gathered for interrogation. " << counterAdvisors << " were detained.";
}

void CK3::World::congregateDFCounties()
{
	auto counter = 0;
	// We're linking all contained counties for a title's tree under that title.
	// This will form actual EU4 tag and contained provinces.
	for (const auto& title: independentTitles)
	{
		title.second->congregateDFCounties();
		for (const auto& province: title.second->getOwnedDFCounties())
		{
			province.second->loadHoldingTitle(std::pair(title.first, title.second));
		}
		counter += static_cast<int>(title.second->getOwnedDFCounties().size());
	}
	Log(LogLevel::Info) << "<> " << counter << " counties held by independents.";
}

void CK3::World::congregateDJCounties()
{
	auto counter = 0;
	// We're linking all dejure provinces under the title as these will be the base
	// for that title's permanent claims, unless already owned.
	for (const auto& title: independentTitles)
	{
		title.second->congregateDJCounties();
		counter += static_cast<int>(title.second->getOwnedDJCounties().size());
	}
	Log(LogLevel::Info) << "<> " << counter << " de jure provinces claimed by independents.";
}

void CK3::World::filterLandlessTitles()
{
	auto counter = 0;
	std::set<std::string> titlesForDisposal;
	for (const auto& title: independentTitles)
	{
		if (title.second->getOwnedDFCounties().empty())
		{
			titlesForDisposal.insert(title.first);
		}
	}
	for (const auto& drop: titlesForDisposal)
	{
		independentTitles.erase(drop);
		counter++;
	}
	Log(LogLevel::Info) << "<> " << counter << " empty titles dropped, " << independentTitles.size() << " remain.";
}

void CK3::World::setElectors()
{
	// Finding electorates is not entirely trivial. CK3 has 7-XX slots, one of which is usually the Emperor himself, but
	// he is not considered an elector in EU4 sense unless he holds one of the electorate titles which are historical.
	// However, CK3 doesn't care about titles, it stores people, so a multiduke with a secondary electoral title will still
	// be elector and we need to flag his primary title as electorate one, as other duchies will possibly be annexed or PU-d.
	// Moreover, these electors may not even be indeps after HRE shattering as player may opt to keep kingdoms but electors were
	// under these kings. We can't help that.

	if (!hreTitle)
	{
		Log(LogLevel::Info) << ">< HRE does not exist.";
		return;
	}
	auto electors = hreTitle->second->getElectors();
	if (electors.empty())
	{
		Log(LogLevel::Info) << ">< HRE does not have electors.";
		return;
	}

	auto counter = 0;

	// Preambule done, we start here.
	// Make a registry of indep titles and their holders.
	std::map<int, std::map<std::string, std::shared_ptr<Title>>> holderTitles; // holder/titles
	std::pair<int, std::shared_ptr<Character>> hreHolder;

	for (const auto& title: independentTitles)
	{
		holderTitles[title.second->getHolder()->first].insert(title);
		if (title.second->isHREEmperor())
		{
			hreHolder = *title.second->getHolder();
		}
	}

	if (!hreHolder.first)
	{
		Log(LogLevel::Info) << ">< HRE does not have an emperor.";
		return;
	}

	// Now roll through electors and flag their primary titles as electors. If kings get electorate status
	// but kingdoms are also shattered, tough luck? Their primary duchy won't inherit electorate as they could
	// end up with multiple electorates, and that's possible only through EU4 gameplay and causes massive
	// penalties to IA.

	for (auto& elector: electors)
	{
		if (electors.size() > 7 && elector.first == hreHolder.first)
		{
			continue; // We're skipping the emperor for 8+ slot setups.
		}

		// How many indep titles does he hold? If any?
		const auto& regItr = holderTitles.find(elector.first);
		if (regItr == holderTitles.end())
		{
			continue; // This fellow was cheated out of electorate titles.
		}

		// Which title is his primary? The first one in his domain (that survived the shattering)
		if (elector.second->getDomain() && !elector.second->getDomain()->getDomain().empty())
		{
			for (const auto& electorTitle: elector.second->getDomain()->getDomain())
			{
				// mark this title as electorate if it's independent and has land.
				if (regItr->second.count(electorTitle.second->getName()) && !electorTitle.second->getOwnedDFCounties().empty())
				{
					electorTitle.second->setElectorate();
					Log(LogLevel::Debug) << "Setting electorate: " << electorTitle.second->getName();
					counter++;
					break;
				}
			}
			// If we marked none here, then all his titles are dependent and he's not a good elector choice.
		}
		else
		{
			// This is a fellow without a domain? Mark some independent non-landless title as electorate.
			for (const auto& title: regItr->second)
			{
				if (!title.second->getOwnedDFCounties().empty())
				{
					title.second->setElectorate();
					Log(LogLevel::Debug) << "Setting electorate: " << title.first;
					counter++;
					break;
				}
			}
			// otherwise no helping this fellow.
		}
	}

	Log(LogLevel::Info) << "<> " << counter << " electorates linked.";
}
