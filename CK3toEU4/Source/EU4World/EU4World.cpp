#include "EU4World.h"
#include "Log.h"
#include <filesystem>
namespace fs = std::filesystem;
#include "../Configuration/Configuration.h"
#include "OSCompatibilityLayer.h"
#include <fstream>
#include "../CK3World/Titles/Title.h"
#include "../CK3World/Characters/Character.h"
#include "Province/EU4Province.h"
#include "../CK3World/Geography/ProvinceHolding.h"
#include "../CK3World/Geography/CountyDetail.h"
#include <cmath>

EU4::World::World(const CK3::World& sourceWorld, const Configuration& theConfiguration, const mappers::ConverterVersion& converterVersion)
{
	auto invasion = theConfiguration.getSunset() == Configuration::SUNSET::ACTIVE;
	
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
	importVanillaCountries(theConfiguration.getEU4Path(), invasion);
	Log(LogLevel::Progress) << "55 %";

	// Which happens now. Translating incoming titles into EU4 tags, with new tags being added to our countries.
	importCK3Countries(sourceWorld);
	
	Log(LogLevel::Progress) << "56 %";
	// Now we can deal with provinces since we know to whom to assign them. We first import vanilla province data.
	// Some of it will be overwritten, but not all.
	importVanillaProvinces(theConfiguration.getEU4Path(), invasion);
	Log(LogLevel::Progress) << "57 %";

	// We can link provinces to regionMapper's bindings, though this is not used at the moment.
	regionMapper->linkProvinces(provinces);
	Log(LogLevel::Progress) << "58 %";

	// Next we import ck2 provinces and translate them ontop a significant part of all imported provinces.
	importCK3Provinces(sourceWorld);
	Log(LogLevel::Progress) << "59 %";

	// With Ck2 provinces linked to those eu4 provinces they affect, we can adjust eu4 province dev values.
	if (theConfiguration.getDevelopment() == Configuration::DEVELOPMENT::IMPORT)
		alterProvinceDevelopment();
	Log(LogLevel::Progress) << "60 %";


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

	if (Utils::DoesFolderExist("blankMod/output/common/country_tags/"))
	{
		auto fileNames = Utils::GetAllFilesInFolder("blankMod/output/common/country_tags/");
		for (const auto& file: fileNames)
		{
			std::ifstream blankCountriesFile(fs::u8path("blankMod/output/common/country_tags/" + file));
			if (!blankCountriesFile.is_open())
				throw std::runtime_error("Could not open blankMod/output/common/country_tags/"+ file + "!");
			loadCountriesFromSource(blankCountriesFile, "blankMod/output/", false);
			blankCountriesFile.close();
		}
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

void EU4::World::importVanillaProvinces(const std::string& eu4Path, bool invasion)
{
	LOG(LogLevel::Info) << "-> Importing Vanilla Provinces";
	// ---- Loading history/provinces
	auto fileNames = Utils::GetAllFilesInFolder(eu4Path + "/history/provinces/");
	for (const auto& fileName: fileNames)
	{
		if (fileName.find(".txt") == std::string::npos)
			continue;
		try
		{
			const auto id = std::stoi(fileName);
			auto newProvince = std::make_shared<Province>(id, eu4Path + "/history/provinces/" + fileName);
			if (provinces.count(id))
			{
				Log(LogLevel::Warning) << "Vanilla province duplication - " << id << " already loaded! Overwriting.";
				provinces[id] = newProvince;
			}
			else
				provinces.insert(std::pair(id, newProvince));
		}
		catch (std::exception& e)
		{
			Log(LogLevel::Warning) << "Invalid province filename: " << eu4Path << "/history/provinces/" << fileName << " : " << e.what();
		}
	}
	LOG(LogLevel::Info) << ">> Loaded " << provinces.size() << " province definitions.";
	if (invasion)
	{
		fileNames = Utils::GetAllFilesInFolder("configurables/sunset/history/provinces/");
		for (const auto& fileName: fileNames)
		{
			if (fileName.find(".txt") == std::string::npos)
				continue;
			auto id = std::stoi(fileName);
			const auto& provinceItr = provinces.find(id);
			if (provinceItr != provinces.end())
				provinceItr->second->updateWith("configurables/sunset/history/provinces/" + fileName);
		}
	}
}

void EU4::World::importCK3Provinces(const CK3::World& sourceWorld)
{
	LOG(LogLevel::Info) << "-> Importing CK3 Provinces";
	auto counter = 0;
	// CK3 provinces map to a subset of eu4 provinces. We'll only rewrite those we are responsible for.
	for (const auto& province: provinces)
	{
		const auto& ck3Titles = provinceMapper.getCK3Titles(province.first);
		// Provinces we're not affecting will not be in this list.
		if (ck3Titles.empty())
			continue;
		// Next, we find what province to use as its initializing source.
		const auto& sourceProvince = determineProvinceSource(ck3Titles, sourceWorld);
		if (!sourceProvince)
		{
			Log(LogLevel::Warning) << "Mismap on CK3 province import for EU4 province: " << province.first;
			continue; // MISMAP, or simply have mod provinces loaded we're not using.
		}
		else
		{
			// And finally, initialize it.
			province.second->initializeFromCK3Title(sourceProvince->second, cultureMapper, religionMapper);
			counter++;
		}
	}
	LOG(LogLevel::Info) << ">> " << counter << " EU4 provinces have been imported from CK3.";
}

std::optional<std::pair<std::string, std::shared_ptr<CK3::Title>>> EU4::World::determineProvinceSource(const std::map<std::string, std::shared_ptr<CK3::Title>>& ck3Titles,
	 const CK3::World& sourceWorld) const
{
	// determine ownership by province development.
	std::map<long long, std::map<std::string, std::shared_ptr<CK3::Title>>> theClaims; // holderID, offered province sources
	std::map<long long, double> theShares;																  // title, development
	long long winner = -1;
	double maxDev = -1;

	// We have multiple titles, c_ level, in battle royale to see which becomes canonical source for a target EU4 province.
	// Since all source titles have working title pointers and have been verified by provinceMapper, we can be pretty relaxed
	// about their integrity. None hail from wastelands or lakes or other junk.
	for (auto ck3Title: ck3Titles)
	{
		if (!ck3Title.second->getHoldingTitle().second)
			throw std::runtime_error(ck3Title.first + " has no holding title?");
		if (!ck3Title.second->getHoldingTitle().second->getHolder())
			throw std::runtime_error(ck3Title.first + " has no holding title holder?");
		const auto holderID = ck3Title.second->getHoldingTitle().second->getHolder()->first; // this is the fellow owning this land.
		theClaims[holderID].insert(ck3Title);
		theShares[holderID] += ck3Title.second->getBuildingWeight(devWeightsMapper);

		// While at it, is this province especially important? Enough so we'd sidestep regular rules?
		// Check for capital provinces
		const auto& capitalBarony = ck3Title.second->getHoldingTitle().second->getHolder()->second->getDomain()->getRealmCapital();		
		if (capitalBarony.second->getDFLiege()->second->getName() == ck3Title.second->getName())
		{
			// This is the someone's capital, don't assign it away if unnecessary.
			theShares[holderID] += 20; // Dev can go up to 50+, so yes, assign it away if someone has overbuilt a nearby province.
			ck3Title.second->setHolderCapital();
		}
		// Check for HRE emperor
		if (ck3Title.second->getHoldingTitle().second->isHREEmperor())
		{
			const auto& HRECapitalBarony = ck3Title.second->getHoldingTitle().second->getHolder()->second->getDomain()->getRealmCapital();
			if (HRECapitalBarony.second->getDFLiege()->second->getName() == ck3Title.second->getName())
			{
				// This is the empire capital, never assign it away.
				theShares[holderID] += 999;
				ck3Title.second->setHRECapital();
			}
		}
	}
	// Let's see who the lucky winner is.
	for (const auto& share: theShares)
	{
		if (share.second > maxDev)
		{
			winner = share.first;
			maxDev = share.second;
		}
	}
	if (winner < 0)
	{
		return std::nullopt;
	}

	// Now that we have a winning title, let's find its largest province to use as a source.
	maxDev = -1; // We can have winning provinces with weight = 0;

	std::pair<std::string, std::shared_ptr<CK3::Title>> toReturn;
	for (const auto& title: theClaims[winner])
	{
		auto provinceWeight = title.second->getBuildingWeight(devWeightsMapper);
		if (title.second->isHolderCapital())
			provinceWeight += 20;
		if (title.second->isHRECapital())
			provinceWeight += 999;

		if (provinceWeight > maxDev)
		{
			toReturn = title;
			maxDev = provinceWeight;
		}
	}
	if (toReturn.first.empty() || !toReturn.second)
	{
		return std::nullopt;
	}
	return toReturn;
}

void EU4::World::alterProvinceDevelopment()
{
	Log(LogLevel::Info) << "-- Scaling Imported provinces";

	auto totalVanillaDev = 0;
	auto totalCK2Dev = 0;
	auto counter = 0;

	for (const auto& province: provinces)
	{
		if (!province.second->getSourceProvince())
			continue;
		totalVanillaDev += province.second->getDev();
		auto adm = 0.0;
		auto dip = 0.0;
		auto mil = 0.0;
		const auto& baronies = province.second->getSourceProvince()->getDFVassals();
		for (const auto& barony: baronies)
		{
			const auto& provinceData = barony.second->getClay()->getProvince()->second;
			if (provinceData->getHoldingType().empty())
				continue;
			const auto buildingNumber = static_cast<double>(provinceData->countBuildings());
			const auto baronyDev = devWeightsMapper.getDevFromHolding() + devWeightsMapper.getDevFromBuilding() * buildingNumber;
			if (provinceData->getHoldingType() == "tribal_holding")
			{
				mil += baronyDev;
			}
			else if (provinceData->getHoldingType() == "city_holding")
			{
				dip += baronyDev;
			}
			else if (provinceData->getHoldingType() == "church_holding")
			{
				adm += baronyDev;
			}
			else if (provinceData->getHoldingType() == "castle_holding")
			{
				adm += baronyDev * 1 / 3; // third to adm
				mil += baronyDev * 2 / 3; // two thirds to mil
			}
		}
		double generalDevelopment = province.second->getSourceProvince()->getClay()->getCounty()->second->getDevelopment();
		generalDevelopment *= devWeightsMapper.getDevFromDev() / 3;
		province.second->setAdm(std::max(static_cast<int>(std::lround(adm + generalDevelopment)), 1));
		province.second->setDip(std::max(static_cast<int>(std::lround(dip + generalDevelopment)), 1));
		province.second->setMil(std::max(static_cast<int>(std::lround(mil + generalDevelopment)), 1));
		counter++;
		totalCK2Dev += province.second->getDev();
	}

	Log(LogLevel::Info) << "<> " << counter << " provinces scaled: " << totalCK2Dev << " development imported (vanilla had " << totalVanillaDev << ").";
}
