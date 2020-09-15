#include "EU4World.h"
#include "Log.h"
#include <filesystem>
namespace fs = std::filesystem;
#include "../Configuration/Configuration.h"
#include "OSCompatibilityLayer.h"
#include <fstream>
#include "../CK3World/Titles/Title.h"
#include "../CK3World/Characters/Character.h"

EU4::World::World(const CK3::World& sourceWorld, const Configuration& theConfiguration, const mappers::ConverterVersion& converterVersion)
{
	LOG(LogLevel::Info) << "*** Hello EU4, let's get painting. ***";
	// Scraping localizations from CK3 so we may know proper names for our countries and people.
	LOG(LogLevel::Info) << "-> Reading Words";
	localizationMapper.scrapeLocalizations(theConfiguration, sourceWorld.getMods().getMods());
	Log(LogLevel::Progress) << "50 %";

	// Scrape Primary Tags for nationalities
	LOG(LogLevel::Info) << "-> Sifting Through EU4 Cultures";
	primaryTagMapper.loadPrimaryTags(theConfiguration);
	Log(LogLevel::Progress) << "51 %";

	// This is our region mapper for eu4 regions, areas and superRegions. It's a pointer because we need
	// to embed it into every cultureMapper individual mapping. It works faster that way.
	LOG(LogLevel::Info) << "-> Initializing Geography";
	regionMapper = std::make_shared<mappers::RegionMapper>();
	regionMapper->loadRegions(theConfiguration);
	Log(LogLevel::Progress) << "52 %";

	// And this is the cultureMapper. It's of vital importance.
	cultureMapper.loadRegionMapper(regionMapper);
	Log(LogLevel::Progress) << "53 %";

	// Our provincemapper is useless for this game. We don't care about baronies, we want <Title> counties.
	LOG(LogLevel::Info) << "-> Injecting Geopolitical Nuances Into Clay";
	provinceMapper.transliterateMappings(sourceWorld.getTitles().getTitles());

	// We start conversion by importing vanilla eu4 countries, history and common sections included.
	// We'll overwrite some of them with ck3 imports.
	importVanillaCountries(theConfiguration.getEU4Path(), theConfiguration.getSunset() == Configuration::SUNSET::ACTIVE);
	Log(LogLevel::Progress) << "55 %";

	// Which happens now. Translating incoming titles into EU4 tags, with new tags being added to our countries.
	importCK3Countries(sourceWorld);
	Log(LogLevel::Progress) << "56 %";

	// And finally, the Dump.
	LOG(LogLevel::Info) << "---> The Dump <---";
	modFile.outname = theConfiguration.getOutputName();
	output(converterVersion, theConfiguration, sourceWorld);
	LOG(LogLevel::Info) << "*** Farewell EU4, granting you independence. ***";
}

void EU4::World::importVanillaCountries(const std::string& eu4Path, bool invasion)
{
	LOG(LogLevel::Info) << "-> Importing Vanilla Countries";
	// ---- Loading common/countries/
	std::ifstream eu4CountriesFile(fs::u8path(eu4Path + "/common/country_tags/00_countries.txt"));
	if (!eu4CountriesFile.is_open())
		throw std::runtime_error("Could not open " + eu4Path + "/common/country_tags/00_countries.txt!");
	loadCountriesFromSource(eu4CountriesFile, eu4Path, true);
	eu4CountriesFile.close();
	if (Utils::DoesFileExist("blankMod/output/common/country_tags/01_special_tags.txt"))
	{
		std::ifstream blankCountriesFile(fs::u8path("blankMod/output/common/country_tags/01_special_tags.txt"));
		if (!blankCountriesFile.is_open())
			throw std::runtime_error("Could not open blankMod/output/common/country_tags/01_special_tags.txt!");
		loadCountriesFromSource(blankCountriesFile, "blankMod/output/", false);
		blankCountriesFile.close();
	}
	if (invasion)
	{
		std::ifstream sunset(fs::u8path("configurables/sunset/common/country_tags/zz_countries.txt"));
		if (!sunset.is_open())
			throw std::runtime_error("Could not open configurables/sunset/common/country_tags/zz_countries.txt!");
		loadCountriesFromSource(sunset, "configurables/sunset/", true);
		sunset.close();
	}

	LOG(LogLevel::Info) << ">> Loaded " << countries.size() << " countries.";

	LOG(LogLevel::Info) << "-> Importing Vanilla Country History";
	// ---- Loading history/countries/
	auto fileNames = Utils::GetAllFilesInFolder(eu4Path + "/history/countries/");
	for (const auto& fileName: fileNames)
	{
		auto tag = fileName.substr(0, 3);
		countries[tag]->loadHistory(eu4Path + "/history/countries/" + fileName);
	}
	// Now our special tags.
	fileNames = Utils::GetAllFilesInFolder("blankMod/output/history/countries/");
	for (const auto& fileName: fileNames)
	{
		auto tag = fileName.substr(0, 3);
		countries[tag]->loadHistory("blankMod/output/history/countries/" + fileName);
	}
	if (invasion)
	{
		fileNames = Utils::GetAllFilesInFolder("configurables/sunset/history/countries/");
		for (const auto& fileName: fileNames)
		{
			auto tag = fileName.substr(0, 3);
			countries[tag]->setSunsetCountry(true);
			countries[tag]->clearHistoryLessons();
			countries[tag]->loadHistory("configurables/sunset/history/countries/" + fileName);
		}
	}
	LOG(LogLevel::Info) << ">> Loaded " << fileNames.size() << " history files.";
}

void EU4::World::loadCountriesFromSource(std::istream& theStream, const std::string& sourcePath, bool isVanillaSource)
{
	while (!theStream.eof())
	{
		std::string line;
		getline(theStream, line);

		if (line[0] == '#' || line.length() < 4)
			continue;
		auto tag = line.substr(0, 3);

		// All file paths are in quotes. The ones outside are commented, so we can use those as markers.
		auto quoteLoc = line.find_first_of('\"');
		auto countryLine = line.substr(quoteLoc + 1, line.length());
		quoteLoc = countryLine.find_last_of('\"');
		countryLine = countryLine.substr(0, quoteLoc);
		const auto filePath = sourcePath + "/common/" + countryLine;

		// We're soaking up all vanilla countries with all current definitions.
		const auto newCountry = std::make_shared<Country>(tag, filePath);
		if (countries.count(tag))
			countries[tag] = newCountry; // Overriding vanilla EU4 with our definitions.
		else
			countries.insert(std::make_pair(tag, newCountry));
		if (!isVanillaSource)
			specialCountryTags.insert(tag);
	}
}

void EU4::World::importCK3Countries(const CK3::World& sourceWorld)
{
	LOG(LogLevel::Info) << "-> Importing CK3 Countries";

	// countries holds all tags imported from EU4. We'll now overwrite some and
	// add new ones from ck3 titles.
	for (const auto& title: sourceWorld.getIndeps())
	{
		if (title.second->getLevel() != CK3::LEVEL::EMPIRE)
			continue;
		importCK3Country(title, sourceWorld);
	}
	for (const auto& title: sourceWorld.getIndeps())
	{
		if (title.second->getLevel() != CK3::LEVEL::KINGDOM)
			continue;
		importCK3Country(title, sourceWorld);
	}
	for (const auto& title: sourceWorld.getIndeps())
	{
		if (title.second->getLevel() != CK3::LEVEL::DUCHY)
			continue;
		importCK3Country(title, sourceWorld);
	}
	for (const auto& title: sourceWorld.getIndeps())
	{
		if (title.second->getLevel() != CK3::LEVEL::COUNTY)
			continue;
		importCK3Country(title, sourceWorld);
	}
	LOG(LogLevel::Info) << ">> " << countries.size() << " total countries recognized.";
}

void EU4::World::importCK3Country(const std::pair<std::string, std::shared_ptr<CK3::Title>>& title, const CK3::World& sourceWorld)
{
	// Grabbing the capital, if possible
	int eu4CapitalID = 0;
	const auto& holderDomain = title.second->getHolder()->second->getDomain();
	if (holderDomain && holderDomain->getRealmCapital().first)
	{
		const auto& capitalBarony = holderDomain->getRealmCapital();
		const auto& capitalCounty = capitalBarony.second->getDJLiege();
		if (capitalCounty)
		{
			const auto& capitalMatch = provinceMapper.getEU4ProvinceNumbers(capitalCounty->second->getName());
			if (!capitalMatch.empty())
				eu4CapitalID = *capitalMatch.begin();			
		}
	}

	// Mapping the title to a tag
	// The Pope is Special! This is land /owned by/ a pope, but might be e_france or k_jerusalem.
	// First title will map to PAP/FAP and others will merge into it.
	std::optional<std::string> tag;
	if (title.second->isThePope())
	{
		tag = titleTagMapper.getTagForTitle("The Pope", eu4CapitalID);
	}
	else
	{
		tag = titleTagMapper.getTagForTitle(title.first, eu4CapitalID);
	}
	if (!tag)
		throw std::runtime_error("Title " + title.first + " could not be mapped!");

	// Locating appropriate existing country
	const auto& countryItr = countries.find(*tag);
	if (countryItr != countries.end())
	{
		countryItr->second->initializeFromTitle(*tag,
			 title,
			 governmentsMapper,
			 religionMapper,
			 cultureMapper,
			 provinceMapper,
			 localizationMapper,
			 rulerPersonalitiesMapper,
			 sourceWorld.getConversionDate());
		title.second->loadEU4Tag(std::pair(*tag, countryItr->second));
	}
	else
	{
		// Otherwise create the country
		auto newCountry = std::make_shared<Country>();
		newCountry->initializeFromTitle(*tag,
			 title,
			 governmentsMapper,
			 religionMapper,
			 cultureMapper,
			 provinceMapper,
			 localizationMapper,
			 rulerPersonalitiesMapper,
			 sourceWorld.getConversionDate());
		title.second->loadEU4Tag(std::pair(*tag, newCountry));
		countries.insert(std::pair(*tag, newCountry));
	}
}
