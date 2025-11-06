#include "EU4World.h"
#include "../CK3World/Characters/Character.h"
#include "../CK3World/Confederations/Confederation.h"
#include "../CK3World/Geography/CountyDetail.h"
#include "../CK3World/Geography/ProvinceHolding.h"
#include "../CK3World/Religions/Faith.h"
#include "../CK3World/Religions/Religion.h"
#include "../CK3World/Titles/Title.h"
#include "../Configuration/Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "Province/EU4Province.h"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <ranges>
namespace fs = std::filesystem;

EU4::World::World(const CK3::World& sourceWorld, const Configuration& theConfiguration, const commonItems::ConverterVersion& converterVersion)
{
	auto invasion = theConfiguration.getSunset() == Configuration::SUNSET::ACTIVE;
	auto dynasticNames = theConfiguration.getDynasticNames() == Configuration::DYNASTICNAMES::ENABLED;
	auto CK3FS = commonItems::ModFilesystem(theConfiguration.getCK3Path(), sourceWorld.getMods());

	// Load correct province mappings.
	bool initProvinceMapper = false;
	for (const auto& mod: sourceWorld.getMods())
	{
		if (commonItems::DoesFileExist("configurables/" + mod.name + "_province_mappings.txt"))
		{
			Log(LogLevel::Info) << ">> Found matching province mappings for mod [" << mod.name << "], loading.";
			provinceMapper = mappers::ProvinceMapper("configurables/" + mod.name + "_province_mappings.txt");
			initProvinceMapper = true;
			break;
		}
	}
	if (!initProvinceMapper)
		provinceMapper = mappers::ProvinceMapper("configurables/province_mappings.txt");

	Log(LogLevel::Info) << "*** Hello EU4, let's get painting. ***";
	localizationMapper = sourceWorld.getLocalizationMapper();
	cultureMapper = sourceWorld.getCultureMapper();
	Log(LogLevel::Progress) << "50 %";

	// Scrape Primary Tags for nationalities
	Log(LogLevel::Info) << "-> Sifting Through EU4 Cultures";
	primaryTagMapper.loadPrimaryTags(theConfiguration);
	Log(LogLevel::Info) << "-> Slurping EU4 Cultures";
	cultureDefinitionsMapper.loadDefinitionsFromEU4Installation(theConfiguration);
	cultureDefinitionsMapper.loadHeritagesFromDisk();
	Log(LogLevel::Info) << "-> Building Dynamic EU4 Culture Definitions";
	cultureDefinitionsMapper.buildDefinitions(cultureMapper);
	Log(LogLevel::Progress) << "51 %";

	// This is our region mapper for eu4 regions, areas and superRegions. It's a pointer because we need
	// to embed it into every cultureMapper individual mapping. It works faster that way.
	Log(LogLevel::Info) << "-> Initializing Geography";
	regionMapper = std::make_shared<mappers::RegionMapper>();
	regionMapper->loadRegions(theConfiguration);
	Log(LogLevel::Progress) << "52 %";

	// And this is the cultureMapper. It's of vital importance.
	cultureMapper.loadRegionMapper(regionMapper);
	Log(LogLevel::Progress) << "53 %";

	// Our provincemapper is useless for this game. We don't care about baronies, we want <Title> counties.
	Log(LogLevel::Info) << "-> Injecting Geopolitical Nuances Into Clay";
	provinceMapper.transliterateMappings(sourceWorld.getTitles().getTitles());

	// Verify all incoming c_counties have a mapping that's not empty.
	Log(LogLevel::Info) << "-> Verifying all county mappings exist.";
	verifyAllCountyMappings(sourceWorld.getTitles().getTitles());

	// Import CK3 dynamic faiths and register them in religionMapper
	Log(LogLevel::Info) << "-> Importing Dynamic Faiths";
	religionMapper.scrapeCK3Religions(CK3FS);
	religionMapper.importCK3Faiths(sourceWorld.getFaiths(), religionDefinitionMapper, religionGroupScraper, localizationMapper);
	Log(LogLevel::Progress) << "54 %";

	// We start conversion by importing vanilla eu4 countries, history and common sections included.
	// We'll overwrite some of them with ck3 imports.
	importVanillaCountries(theConfiguration.getEU4Path(), invasion);
	Log(LogLevel::Progress) << "55 %";

	// Which happens now. Translating incoming titles into EU4 tags, with new tags being added to our countries.
	importCK3Countries(sourceWorld, theConfiguration.getStartDateOption(), startDateMapper.getStartDate(), dynasticNames);

	Log(LogLevel::Progress) << "56 %";
	// Now we can deal with provinces since we know to whom to assign them. We first import vanilla province data.
	// Some of it will be overwritten, but not all.
	importVanillaProvinces(theConfiguration.getEU4Path(), invasion);
	Log(LogLevel::Progress) << "57 %";

	// We can link provinces to regionMapper's bindings, though this is not used at the moment.
	Log(LogLevel::Info) << "-> Linking Provinces to Regions";
	regionMapper->linkProvinces(provinces);
	Log(LogLevel::Progress) << "58 %";

	// Next we import ck provinces and translate them ontop a significant part of all imported provinces.
	importCK3Provinces(theConfiguration.getDiscoveredBy());
	Log(LogLevel::Progress) << "59 %";

	// With Ck provinces linked to those eu4 provinces they affect, we can adjust eu4 province dev values.
	if (theConfiguration.getDevelopment() == Configuration::DEVELOPMENT::IMPORT)
		alterProvinceDevelopment(theConfiguration.getMultiProvDevTransfer() == Configuration::MULTIPROVDEVTRANSFER::MP);
	Log(LogLevel::Progress) << "60 %";

	// We then link them to their respective countries. Those countries that end up with 0 provinces are defacto dead.
	Log(LogLevel::Info) << "-> Linking Provinces to Countries";
	linkProvincesToCountries();
	Log(LogLevel::Progress) << "61 %";

	// Country capitals are fuzzy, and need checking if we assigned them to some other country during mapping.
	verifyCapitals();
	Log(LogLevel::Progress) << "62 %";

	// This step is important. CK2 data was sketchy and not every character or province has culture/religion data.
	// In CK3 this is not the case, but if we cannot craft dynamic faiths or cannot convert existing religions/cultures due to
	// mismaps this is the recovery procedure.
	// For those, we look at vanilla provinces and override missing bits with vanilla setup. Yeah, a bit more sunni in
	// hordeland, but it's fine.
	Log(LogLevel::Info) << "-> Verifying Religions and Cultures";
	verifyReligionsAndCultures();

	Log(LogLevel::Info) << "-> Initializing Dynamic National Ideas";
	generateNationalIdeasFromDynamicCultures(sourceWorld.getCultures());

	Log(LogLevel::Progress) << "63 %";
	// With all provinces and rulers religion/culture set, only now can we import advisers, which also need religion/culture set.
	// Those advisers coming without such data use the monarch's religion/culture.
	importAdvisers();
	Log(LogLevel::Progress) << "64 %";

	// Rulers with multiple crowns either get PU agreements, or just annex the other crowns.
	resolvePersonalUnions();

	if (theConfiguration.getTribalConfederations() == Configuration::TRIBALCONFEDERATIONS::MERGE)
		mergeConfederations(sourceWorld.getConfederations());
	Log(LogLevel::Progress) << "65 %";

	// We're onto the finesse part of conversion now. HRE was shattered in CK3 World and now we're assigning electorates, free
	// cities, and such.
	Log(LogLevel::Info) << "-> Distributing HRE Subtitles";
	distributeHRESubtitles(theConfiguration);

	if (sourceWorld.getHRETitle())
	{
		Log(LogLevel::Info) << "-> Marking HRE Title";
		markHRETag(theConfiguration,
			 *sourceWorld.getHRETitle(),
			 sourceWorld.getConversionDate(),
			 startDateMapper.getStartDate(),
			 theConfiguration.getStartDateOption());
	}
	Log(LogLevel::Progress) << "66 %";

	// Mark the MoH.
	Log(LogLevel::Info) << "-> Marking Mandate of Heaven Holder.";
	markCelestialTitle(theConfiguration, sourceWorld.getCelestialTitle());

	// With all religious/cultural matters taken care of, we can now set reforms
	Log(LogLevel::Info) << "-> Assigning Country Reforms";
	assignAllCountryReforms();
	Log(LogLevel::Progress) << "67 %";

	// Vassalages were set in ck3::world but we have to transcribe those into EU4 agreements.
	// Ck3 does not support tributaries as such and we care not about alliances.
	Log(LogLevel::Info) << "-> Importing Vassals";
	diplomacy.importVassals(countries);

	Log(LogLevel::Info) << "-> Generating Tributaries";
	diplomacy.generateTributaries(countries);

	Log(LogLevel::Info) << "-> Coalescing Hordes";
	coalesceHordes();
	Log(LogLevel::Progress) << "68 %";

	// We're distributing permanent claims according to dejure distribution.
	distributeClaims(theConfiguration);

	// We're distributing dead cores according to cultures.
	distributeDeadCores();
	Log(LogLevel::Progress) << "69 %";

	// Now for the final tweaks.
	distributeForts();
	Log(LogLevel::Progress) << "70 %";

	// Tengri
	fixTengri();
	Log(LogLevel::Progress) << "71 %";

	// Check for duplicate country names and rename accordingly
	fixDuplicateNames();
	Log(LogLevel::Progress) << "72 %";

	// Siberia
	siberianQuestion(theConfiguration);
	Log(LogLevel::Progress) << "73 %";

	// African Passes
	africanQuestion();
	Log(LogLevel::Progress) << "74 %";

	// Mongolian Question
	mongolianQuestion();

	// No-Islam worlds
	religiousQuestion(sourceWorld.doesIslamExist());
	Log(LogLevel::Progress) << "75 %";

	// Filtering dead relationships
	diplomacy.filterDeadRelationships(countries);

	Log(LogLevel::Info) << "-- Crafting Flags";
	flagFoundry.loadImageFolders(theConfiguration, sourceWorld.getMods());
	flagFoundry.generateFlags(countries, theConfiguration, religionMapper.getGeneratedReligions(), sourceWorld.getMods());
	Log(LogLevel::Progress) << "76 %";

	// And finally, the Dump.
	Log(LogLevel::Info) << "---> The Dump <---";
	modFile.outname = theConfiguration.getOutputName();
	modFile.version = converterVersion.getMaxTarget();
	output(converterVersion, theConfiguration, sourceWorld, startDateMapper.getStartDate());
	Log(LogLevel::Info) << "*** Farewell EU4, granting you independence. ***";
}

void EU4::World::coalesceHordes()
{
	// We need to merge various horde subclans into a single horde entity. We don't care about vassalages or tributaries, and treat both
	// as direct hierarchy when merging.

	auto mergeCounter = 0;

	// Let's filter out all agreements related to hordes so we can iterate faster. We're looking for 2 hordes either in tributary or vassal relations,
	// Ie, mongols merge with mongols, oirats with oirats etc. Chains of 3+ are possible so we'll need to do this recursively.

	std::map<std::string, std::vector<std::shared_ptr<Country>>> hordeDependencies; // overlord -> dependents

	for (const auto& agreement: diplomacy.getAgreements())
	{
		const auto& eu4tag1 = agreement->getFirst();
		const auto& eu4tag2 = agreement->getSecond();
		const auto& country1 = countries.at(eu4tag1);
		const auto& country2 = countries.at(eu4tag2);

		if (!country1->getTitle() || country1->getGovernment() != "tribal" || !country1->getGovernmentReforms().contains("steppe_horde"))
			continue;
		if (!country2->getTitle() || country2->getGovernment() != "tribal" || !country2->getGovernmentReforms().contains("steppe_horde"))
			continue;
		if (!(agreement->getType() == "dependency" && agreement->getSubjectType() == "tributary_state") && agreement->getType() != "vassal")
			continue;

		// We'll brute force circulars. Don't waste time checking.

		if (hordeDependencies.contains(eu4tag1))
		{
			hordeDependencies.at(eu4tag1).emplace_back(country2);
		}
		else
		{
			hordeDependencies.emplace(eu4tag1, std::vector<std::shared_ptr<Country>>{country2});
		}
	}

	// Now let's roll and recursively annex the bunch.

	for (const auto& tag: hordeDependencies | std::views::keys)
	{
		annexHordes(tag, hordeDependencies, std::set{tag});
		mergeCounter++;
	}

	Log(LogLevel::Info) << "<> Coalesced " << mergeCounter << " hordes.";
}

void EU4::World::annexHordes(const std::string& tag,
	 std::map<std::string, std::vector<std::shared_ptr<Country>>>& hordeDependencies,
	 std::set<std::string> seenCountries)
{
	const auto& annexer = countries.at(tag);
	for (const auto& annexee: hordeDependencies.at(tag))
	{
		if (annexee->getProvinces().empty())
			continue; // Already sucked dry.
		if (seenCountries.contains(annexee->getTag()))
		{
			// We've been here before. Yay for circulars. Bail asap.
			continue;
		}
		if (hordeDependencies.contains(annexee->getTag()))
		{
			seenCountries.emplace(annexee->getTag());
			annexHordes(annexee->getTag(), hordeDependencies, seenCountries);
		}
		annexer->annexCountry(std::pair(annexee->getTag(), annexee));
	}
}

void EU4::World::mergeConfederations(const CK3::Confederations& confederations)
{
	// There is no real rhyme or reason about this. Confederations members in CK3 may or may not have survived the transition, and their individual stats are
	// more or less meaningless in the grand scheme. We'll just pick the first one and merge other survivors into it, and then rename it.

	Log(LogLevel::Info) << "-> Merging Confederations";

	auto memberCounter = 0;
	auto confederationCounter = 0;

	for (const auto& confederation: confederations.getConfederations() | std::views::values)
	{
		std::shared_ptr<Country> confederationHolder = nullptr; // this will be the holder for the federation;

		for (const auto& holder: confederation->getMembers() | std::views::values)
		{
			if (!holder)
				continue; // could be deleted over the run.
			// first do a buttload of checks to confirm member is sane.
			const auto& charDomain = holder->getCharacterDomain();
			if (!charDomain)
				continue;
			const auto& domain = charDomain->getDomain();
			if (domain.empty())
				continue;
			const auto& primaryTitle = domain.begin()->second;
			if (!primaryTitle)
				continue;
			const auto& eu4country = primaryTitle->getEU4Tag();
			if (!eu4country || eu4country->first.empty() || !eu4country->second)
				continue;
			if (eu4country->second->getProvinces().empty())
				continue;

			if (!confederationHolder)
			{
				confederationHolder = eu4country->second;
			}
			else
			{
				confederationHolder->annexCountry(*eu4country);
			}
			memberCounter++;
		}

		// did we even do anything?
		if (!confederationHolder)
			continue;

		confederationHolder->renameAndRemask(*confederation);
		confederationCounter++;
	}

	Log(LogLevel::Info) << "<> Merged " << memberCounter << " confederation members into " << confederationCounter << " confederations.";
}

void EU4::World::religiousQuestion(bool doesIslamExist)
{
	if (doesIslamExist)
		return;

	auto counterProvince = 0;
	auto counterCountry = 0;
	// Update the provinces then.
	for (const auto& [provinceID, province]: provinces)
	{
		const auto& replacementReligion = islamOverrideMapper.getReplacementReligionForProvince(provinceID);
		if (replacementReligion)
		{
			province->setReligion(*replacementReligion);
			++counterProvince;
		}
	}
	// Now fix the countries with capitals in said provinces.
	for (const auto& country: countries | std::views::values)
	{
		const auto& capitalID = country->getCapitalID();
		if (!capitalID)
			continue; // shrug
		const auto& replacementReligion = islamOverrideMapper.getReplacementReligionForProvince(capitalID);
		if (replacementReligion)
		{
			country->setReligion(*replacementReligion);
			++counterCountry;
		}
	}
	Log(LogLevel::Info) << ">> Updated " << counterProvince << " provinces and " << counterCountry << " countries.";
}

void EU4::World::verifyAllCountyMappings(const std::map<std::string, std::shared_ptr<CK3::Title>>& ck3Titles) const
{
	for (const auto& title: ck3Titles)
	{
		if (title.second->getLevel() != CK3::LEVEL::COUNTY)
			continue;
		const auto& mappings = provinceMapper.getEU4ProvinceNumbers(title.first);
		if (mappings.empty() && !title.first.starts_with("c_nf_")) // Ignore dynamic titles.
		{
			Log(LogLevel::Warning) << "Province mapping error: " << title.first << " has no EU4 provinces!";
		}
	}
}


void EU4::World::importVanillaCountries(const std::filesystem::path& eu4Path, bool invasion)
{
	Log(LogLevel::Info) << "-> Importing Vanilla Countries";
	// ---- Loading common/countries/
	std::ifstream eu4CountriesFile(eu4Path / "common/country_tags/00_countries.txt");
	if (!eu4CountriesFile.is_open())
		throw std::runtime_error("Could not open " + eu4Path.string() + "common/country_tags/00_countries.txt!");
	loadCountriesFromSource(eu4CountriesFile, eu4Path, true);
	eu4CountriesFile.close();

	if (commonItems::DoesFolderExist("blankMod/output/common/country_tags"))
	{
		auto fileNames = commonItems::GetAllFilesInFolder("blankMod/output/common/country_tags");
		for (const auto& file: fileNames)
		{
			std::ifstream blankCountriesFile("blankMod/output/common/country_tags" / file);
			if (!blankCountriesFile.is_open())
				throw std::runtime_error("Could not open blankMod/output/common/country_tags/" + file.string() + "!");
			loadCountriesFromSource(blankCountriesFile, "blankMod/output", false);
			blankCountriesFile.close();
		}
	}

	if (invasion)
	{
		std::ifstream sunset("configurables/sunset/common/country_tags/zz_countries.txt");
		if (!sunset.is_open())
			throw std::runtime_error("Could not open configurables/sunset/common/country_tags/zz_countries.txt!");
		loadCountriesFromSource(sunset, "configurables/sunset", true);
		sunset.close();
	}

	Log(LogLevel::Info) << ">> Loaded " << countries.size() << " countries.";

	Log(LogLevel::Info) << "-> Importing Vanilla Country History";
	// ---- Loading history/countries/
	auto fileNames = commonItems::GetAllFilesInFolder(eu4Path / "history/countries");
	for (const auto& fileName: fileNames)
	{
		auto tag = fileName.string().substr(0, 3);
		if (countries.contains(tag))
		{
			countries[tag]->loadHistory(eu4Path / "history/countries" / fileName);
		}
		else
		{
			Log(LogLevel::Warning) << "Trying to load " << tag << " from " << fileName << " which isn't defined in any 00_countries.txt!";
		}
	}
	// Now our special tags.
	fileNames = commonItems::GetAllFilesInFolder("blankMod/output/history/countries");
	for (const auto& fileName: fileNames)
	{
		auto tag = fileName.string().substr(0, 3);
		if (countries.contains(tag))
		{
			countries[tag]->loadHistory("blankMod/output/history/countries" / fileName);
		}
		else
		{
			Log(LogLevel::Warning) << "Trying to load " << tag << " from " << fileName << " which isn't defined in blankmod 00_countries.txt!";
		}
	}
	if (invasion)
	{
		fileNames = commonItems::GetAllFilesInFolder("configurables/sunset/history/countries");
		for (const auto& fileName: fileNames)
		{
			auto tag = fileName.string().substr(0, 3);
			if (countries.contains(tag))
			{
				countries[tag]->setSunsetCountry(true);
				countries[tag]->clearHistoryLessons();
				countries[tag]->loadHistory("configurables/sunset/history/countries" / fileName);
			}
			else
			{
				Log(LogLevel::Warning) << "Trying to load sunset " << tag << " from " << fileName << " which isn't defined in sunset's 00_countries.txt!";
			}
		}
	}
	Log(LogLevel::Info) << ">> Loaded " << fileNames.size() << " history files.";
}

void EU4::World::loadCountriesFromSource(std::istream& theStream, const std::filesystem::path& sourcePath, bool isVanillaSource)
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
		if (quoteLoc == std::string::npos)
		{
			Log(LogLevel::Warning) << "Iffy country line: " << line;
			continue;
		}
		auto countryLine = line.substr(quoteLoc + 1, line.length());
		quoteLoc = countryLine.find_last_of('\"');
		if (quoteLoc == std::string::npos)
		{
			Log(LogLevel::Warning) << "Iffy country line: " << line;
			continue;
		}
		countryLine = countryLine.substr(0, quoteLoc);
		const auto filePath = sourcePath / "common" / countryLine;

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

void EU4::World::importCK3Countries(const CK3::World& sourceWorld, Configuration::STARTDATE startDateOption, date startDate, const bool dynasticNames)
{
	Log(LogLevel::Info) << "-> Importing CK3 Countries";
	// countries holds all tags imported from EU4. We'll now overwrite some and
	// add new ones from ck3 titles.
	for (const auto& title: sourceWorld.getIndeps())
	{
		if (title.second->getLevel() != CK3::LEVEL::HEGEMONY)
			continue;
		importCK3Country(title, sourceWorld, startDateOption, startDate, dynasticNames);
	}
	for (const auto& title: sourceWorld.getIndeps())
	{
		if (title.second->getLevel() != CK3::LEVEL::EMPIRE)
			continue;
		importCK3Country(title, sourceWorld, startDateOption, startDate, dynasticNames);
	}
	for (const auto& title: sourceWorld.getIndeps())
	{
		if (title.second->getLevel() != CK3::LEVEL::KINGDOM)
			continue;
		importCK3Country(title, sourceWorld, startDateOption, startDate, dynasticNames);
	}
	for (const auto& title: sourceWorld.getIndeps())
	{
		if (title.second->getLevel() != CK3::LEVEL::DUCHY)
			continue;
		importCK3Country(title, sourceWorld, startDateOption, startDate, dynasticNames);
	}
	for (const auto& title: sourceWorld.getIndeps())
	{
		if (title.second->getLevel() != CK3::LEVEL::COUNTY)
			continue;
		importCK3Country(title, sourceWorld, startDateOption, startDate, dynasticNames);
	}
	Log(LogLevel::Info) << ">> " << countries.size() << " total countries recognized.";
}

void EU4::World::importCK3Country(const std::pair<std::string, std::shared_ptr<CK3::Title>>& title,
	 const CK3::World& sourceWorld,
	 Configuration::STARTDATE startDateOption,
	 date startDate,
	 bool dynasticNames)
{
	// Are we being fed crap?
	if (!title.second)
	{
		Log(LogLevel::Error) << "We're converting " << title.first << " which doesnt even exist! This save is corrupted.";
		return;
	}
	// Grabbing the capital, if possible
	int eu4CapitalID = 0;
	if (title.second->getHolder()->second)
	{
		const auto& holderDomain = title.second->getHolder()->second->getCharacterDomain();
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
	}
	else
	{
		Log(LogLevel::Error) << title.first << " has no holder! This is BAD! Proceeding blind.";
	}

	// Mapping the title to a tag
	// The Pope is Special! This is land /owned by/ a pope, but might be e_france or k_jerusalem.
	// First title will map to PAP/FAP and others will merge into it.
	std::optional<std::string> tag;
	if (title.second->isThePope())
	{
		tag = titleTagMapper.getTagForTitle("The Pope", eu4CapitalID);
	}

	// Next we ask if this is a "china"? Chinas look at special mappings.
	const auto& alteredName = title.second->getAlteredName();
	if (!tag && alteredName)
	{
		if (const auto& china = titleTagMapper.getChinaForTitle(*alteredName); china)
		{
			tag = *china;
		}
		else
		{
			// Yuan and similar, it's possible this is a localization string, "Yuan".
			const auto& keys = localizationMapper.reverseLookup(*alteredName);
			for (const auto& key: keys)
			{
				if (const auto& keyChina = titleTagMapper.getChinaForTitle(key); keyChina)
				{
					tag = *keyChina;
					break;
				}
			}
		}
	}

	// Otherwise proceed as normal.
	if (!tag)
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
			 sourceWorld.getConversionDate(),
			 startDate,
			 startDateOption,
			 dynasticNames);
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
			 sourceWorld.getConversionDate(),
			 startDate,
			 startDateOption,
			 dynasticNames);
		newCountry->setGeneratedNation();
		title.second->loadEU4Tag(std::pair(*tag, newCountry));
		countries.insert(std::pair(*tag, newCountry));
	}
}

void EU4::World::importVanillaProvinces(const std::filesystem::path& eu4Path, bool invasion)
{
	Log(LogLevel::Info) << "-> Importing Vanilla Provinces";
	// ---- Loading history/provinces
	for (const auto& fileName: commonItems::GetAllFilesInFolder(eu4Path / "history/provinces"))
	{
		if (fileName.extension() != ".txt")
			continue;
		if (fileName.string().starts_with("~"))
			continue;
		try
		{
			const auto id = std::stoi(fileName);
			auto newProvince = std::make_shared<Province>(id, eu4Path / "history/provinces" / fileName);
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
	Log(LogLevel::Info) << ">> Loaded " << provinces.size() << " province definitions.";
	if (invasion)
	{
		for (const auto& fileName: commonItems::GetAllFilesInFolder("configurables/sunset/history/provinces"))
		{
			if (fileName.extension() != ".txt")
				continue;
			auto id = std::stoi(fileName);
			const auto& provinceItr = provinces.find(id);
			if (provinceItr != provinces.end())
				provinceItr->second->updateWith("configurables/sunset/history/provinces" / fileName);
		}
	}
}

void EU4::World::importCK3Provinces(Configuration::DISCOVEREDBY discoveredBy)
{
	Log(LogLevel::Info) << "-> Importing CK3 Provinces";
	auto counter = 0;
	// CK3 provinces map to a subset of eu4 provinces. We'll only rewrite those we are responsible for.
	for (const auto& province: provinces)
	{
		const auto& ck3Titles = provinceMapper.getCK3Titles(province.first);
		// Provinces we're not affecting will not be in this list.
		if (ck3Titles.empty())
			continue;
		// Next, we find what province to use as its initializing source.
		const auto& sourceProvince = determineProvinceSource(ck3Titles);
		if (!sourceProvince)
		{
			Log(LogLevel::Warning) << "Mismap on CK3 province import for EU4 province: " << province.first;
			continue; // MISMAP, or simply have mod provinces loaded we're not using.
		}
		else
		{
			// And finally, initialize it.
			province.second->initializeFromCK3Title(sourceProvince->second, cultureMapper, religionMapper, locDegrader, regionMapper, discoveredBy);
			counter++;
		}
	}
	Log(LogLevel::Info) << ">> " << counter << " EU4 provinces have been imported from CK3.";
}

std::optional<std::pair<std::string, std::shared_ptr<CK3::Title>>> EU4::World::determineProvinceSource(
	 const std::map<std::string, std::shared_ptr<CK3::Title>>& ck3Titles) const
{
	// determine ownership by province development.
	std::map<long long, std::map<std::string, std::shared_ptr<CK3::Title>>> theClaims; // holderID, offered province sources
	std::map<long long, double> theShares;															  // title, development
	long long winner = -1;
	double maxDev = -1;

	// We have multiple titles, c_ level, in battle royale to see which becomes canonical source for a target EU4 province.
	// Since all source titles have working title pointers and have been verified by provinceMapper, we can be pretty relaxed
	// about their integrity. None hail from wastelands or lakes or other junk.
	for (auto ck3Title: ck3Titles)
	{
		if (!ck3Title.second)
		{
			Log(LogLevel::Error) << ck3Title.first << " doesn't even exist and isn't mappable to a province. wheee.";
			continue;
		}
		if (!ck3Title.second->getHoldingTitle().second)
		{
			Log(LogLevel::Error) << ck3Title.first << " has no holding title? Did you import this save from an old version?";
			continue;
		}
		if (!ck3Title.second->getHoldingTitle().second->getHolder())
		{
			Log(LogLevel::Error) << ck3Title.first + " has no holding title holder? Did you import this save from an old version?";
			continue;
		}
		const auto holderID = ck3Title.second->getHoldingTitle().second->getHolder()->first; // this is the fellow owning this land.
		theClaims[holderID].insert(ck3Title);
		theShares[holderID] += ck3Title.second->getBuildingWeight(devWeightsMapper);

		// While at it, is this province especially important? Enough so we'd sidestep regular rules?
		// Check for capital provinces but first make sure we're sane.

		if (!ck3Title.second->getHoldingTitle().second->getHolder()->second->getCharacterDomain())
		{
			Log(LogLevel::Warning) << ck3Title.first << "'s holder has no domain, breaking province init!";
			return *ck3Titles.begin();
		}
		if (!ck3Title.second->getHoldingTitle().second->getHolder()->second->getCharacterDomain()->getRealmCapital().second)
		{
			Log(LogLevel::Warning) << ck3Title.first << "'s holder has no realm capital. Likely corruption, breaking province init!";
			return *ck3Titles.begin();
		}

		const auto& capitalBarony = ck3Title.second->getHoldingTitle().second->getHolder()->second->getCharacterDomain()->getRealmCapital();
		if (!capitalBarony.second->getDFLiege() || !capitalBarony.second->getDFLiege()->second)
		{
			Log(LogLevel::Warning) << ck3Title.first << "'s capital barony is unlinked! What?";
			return *ck3Titles.begin();
		}

		if (capitalBarony.second->getDFLiege()->second->getName() == ck3Title.second->getName())
		{
			// This is the someone's capital, don't assign it away if unnecessary.
			theShares[holderID] += 20; // Dev can go up to 50+, so yes, assign it away if someone has overbuilt a nearby province.
			ck3Title.second->setHolderCapital();
		}
		// Check for HRE emperor
		if (ck3Title.second->getHoldingTitle().second->isHREEmperor())
		{
			const auto& HRECapitalBarony = ck3Title.second->getHoldingTitle().second->getHolder()->second->getCharacterDomain()->getRealmCapital();
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

	toReturn.second->pickDisplayName(ck3Titles);
	if (toReturn.first.empty() || !toReturn.second)
	{
		return std::nullopt;
	}
	return std::move(toReturn);
}

void EU4::World::alterProvinceDevelopment(bool absoluteSwitch)
{
	Log(LogLevel::Info) << "-- Scaling Imported provinces";

	auto totalVanillaDev = 0;
	auto totalCK3Dev = 0;
	auto counter = 0;

	for (const auto& province: provinces | std::views::values)
	{
		if (!province->getSourceProvince())
			continue;
		totalVanillaDev += province->getDev();
		double totalAdm = 0;
		double totalDip = 0;
		double totalMil = 0;
		double generalDevelopment = 0;

		if (!absoluteSwitch)
		{
			// grab only the baronies of the source province, excess dev be damned.
			const auto& baronies = province->getSourceProvince()->getDFVassals();
			auto [adm, dip, mil] = sumBaroniesForDevelopment(baronies);
			totalAdm = adm;
			totalDip = dip;
			totalMil = mil;
			const auto development = province->getSourceProvince()->getClay()->getCounty()->second->getDevelopment();
			generalDevelopment = std::max(0.0, development - devWeightsMapper.getDevTreshold());
		}
		else
		{
			// sum every source mapping province's development.
			const auto& sourceTitles = provinceMapper.getCK3Titles(province->getProvinceID());
			for (const auto& sourceTitle: sourceTitles | std::views::values)
			{
				if (!sourceTitle)
					continue;
				const auto& baronies = sourceTitle->getDFVassals();
				auto [adm, dip, mil] = sumBaroniesForDevelopment(baronies);
				totalAdm += adm;
				totalDip += dip;
				totalMil += mil;
				const auto development = sourceTitle->getClay()->getCounty()->second->getDevelopment();
				generalDevelopment += std::max(0.0, development - devWeightsMapper.getDevTreshold());
			}
		}

		double modFactor = 1; // used to alter the results based on source province's target province's number.
		if (absoluteSwitch)
			modFactor = calculateProvinceDevFactor(province);

		generalDevelopment *= modFactor;
		totalAdm *= modFactor;
		totalDip *= modFactor;
		totalMil *= modFactor;

		generalDevelopment *= devWeightsMapper.getDevFromDev() / 3;
		province->setAdm(std::max(static_cast<int>(std::lround(totalAdm + generalDevelopment)), 1));
		province->setDip(std::max(static_cast<int>(std::lround(totalDip + generalDevelopment)), 1));
		province->setMil(std::max(static_cast<int>(std::lround(totalMil + generalDevelopment)), 1));
		counter++;
		totalCK3Dev += province->getDev();
	}

	Log(LogLevel::Info) << "<> " << counter << " provinces scaled: " << totalCK3Dev << " development imported (vanilla had " << totalVanillaDev << ").";
}

double EU4::World::calculateProvinceDevFactor(const std::shared_ptr<Province>& province) const
{
	// algorithm provided by @Clonefusion
	const auto targetProvinces = static_cast<int>(provinceMapper.getEU4ProvinceNumbers(province->getSourceProvince()->getName()).size());
	const auto targetDouble = static_cast<double>(targetProvinces);
	const auto sourceProvinces = static_cast<int>(provinceMapper.getCK3Titles(province->getProvinceID()).size());
	const auto sourceDouble = static_cast<double>(sourceProvinces);

	double modFactor = 1;

	if (targetDouble > 1 && sourceDouble > 1)
	{
		modFactor = 1 / targetDouble;
		if (targetDouble > sourceDouble)
		{
			modFactor *= 1.1;
			if (targetDouble > sourceDouble * 1.5)
			{
				modFactor *= 1.1;
				if (targetDouble > sourceDouble * 2)
					modFactor *= 1.1;
			}
		}
		else if (sourceDouble > targetDouble)
		{
			modFactor *= 0.9;
			if (sourceDouble > targetDouble * 1.5)
			{
				modFactor *= 0.9;
				if (sourceDouble > targetDouble * 2)
					modFactor *= 0.9;
			}
		}
	}
	else if (targetProvinces == 2)
		modFactor = 0.8;
	else if (targetProvinces == 3)
		modFactor = 0.68;
	else if (targetProvinces > 3)
		modFactor = 0.6;
	else if (sourceProvinces == 2)
		modFactor = 0.91;
	else if (sourceProvinces == 3)
		modFactor = 0.83;
	else if (sourceProvinces == 4)
		modFactor = 0.76;
	else if (sourceProvinces == 5)
		modFactor = 0.7;
	else if (sourceProvinces == 6)
		modFactor = 0.65;
	else if (sourceProvinces > 6)
		modFactor = 0.61;

	return modFactor;
}

std::tuple<double, double, double> EU4::World::sumBaroniesForDevelopment(const std::map<long long, std::shared_ptr<CK3::Title>>& baronies) const
{
	auto adm = 0.0;
	auto dip = 0.0;
	auto mil = 0.0;
	for (const auto& barony: baronies | std::views::values)
	{
		if (!barony)
			continue;
		if (!barony->getClay())
			continue;
		if (!barony->getClay()->getProvince())
			continue;
		if (!barony->getClay()->getProvince()->second)
			continue;
		const auto& provinceData = barony->getClay()->getProvince()->second;
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
	return {adm, dip, mil};
}


void EU4::World::linkProvincesToCountries()
{

	// Some of the provinces have linked countries, but new world won't. We need to insert links both ways there.
	for (const auto& province: provinces)
	{
		if (province.second->getOwner().empty())
			continue; // this is the uncolonized case
		const auto& countryItr = countries.find(province.second->getOwner());
		if (countryItr != countries.end())
		{
			// registering owner in province
			if (province.second->getTagCountry().first.empty())
				province.second->registerTagCountry(std::pair(countryItr->first, countryItr->second));
			// registering province in owner.
			countryItr->second->registerProvince(std::pair(province.first, province.second));
		}
		else if (province.second->getOwner() == "XXX") // this is PDX "special" way to mark provinces from out of scope as taken.
		{
			// ignore.
		}
		else
		{
			Log(LogLevel::Warning) << "Province " << province.first << " owner " << province.second->getOwner() << " has no country!";
		}
	}

	// Super Mongolia?
	if (countries.count("MGE") && !countries.find("MGE")->second->getProvinces().empty() && countries.count("KHA"))
	{
		bool isSource = false;
		for (auto province: countries.find("KHA")->second->getProvinces())
		{
			if (province.second->getSourceProvince())
			{
				isSource = true;
				break;
			}
		}
		if (!isSource)
		{
			// Move all Mongolia provinces under Mongol Empire, only happens if Mongolia is seperate from the Mongol Empire
			for (const auto& province: provinces)
			{
				if (province.second->getOwner() == "KHA")
				{
					province.second->setOwner("MGE");
					province.second->setController("MGE");
					province.second->addCore("MGE");
				}
			}

			// Add cores for provinces in China/Manchuria
			provinces.find(4674)->second->addCore("MGE");
			provinces.find(722)->second->addCore("MGE");
			provinces.find(4675)->second->addCore("MGE");
			provinces.find(4672)->second->addCore("MGE");
			provinces.find(702)->second->addCore("MGE");
			provinces.find(701)->second->addCore("MGE");
			provinces.find(4223)->second->addCore("MGE");
		}
	}
}

void EU4::World::verifyCapitals()
{
	Log(LogLevel::Info) << "-- Verifying All countries Have Capitals";

	auto counter = 0;
	for (const auto& country: countries)
	{
		// POPE is special. Of course. Skip this for pope because he may end up with a capital in new world or something.
		if (country.first == "PAP")
			continue;
		if (country.second->verifyCapital(provinceMapper))
			counter++;
	}

	Log(LogLevel::Info) << "<> " << counter << " capitals have been reassigned.";
}

template <typename KeyType, typename ValueType> std::pair<KeyType, ValueType> get_max(const std::map<KeyType, ValueType>& x)
{
	using pairtype = std::pair<KeyType, ValueType>;
	return *std::max_element(x.begin(), x.end(), [](const pairtype& p1, const pairtype& p2) {
		return p1.second < p2.second;
	});
}

void EU4::World::verifyReligionsAndCultures()
{
	// We are checking every country if it lacks primary religion and culture. This is an issue for mismaps mostly.
	// For those lacking setups, we'll do a provincial census and inherit those values.
	for (const auto& country: countries)
	{
		// It's possible to get non-christian countries excommunicated through broken setups. Let's clear those immediately.
		if (country.second->isExcommunicated())
		{
			const auto& religion = country.second->getReligion();
			if (religion != "catholic") // TODO: check for papacies in dynamic faiths.
				country.second->clearExcommunicated();
		}
		// And then proceed on checking the missing boxes.
		if (!country.second->getReligion().empty() && !country.second->getPrimaryCulture().empty() && !country.second->getTechGroup().empty() &&
			 !country.second->getGFX().empty())
			continue;
		if (country.second->getProvinces().empty())
			continue; // No point.

		std::map<std::string, int> religiousCensus;
		std::map<std::string, int> culturalCensus;
		for (const auto& province: country.second->getProvinces())
		{
			if (province.second->getReligion().empty())
			{
				Log(LogLevel::Warning) << "Province " << province.first << " has no religion set!";
				continue;
			}
			if (province.second->getCulture().empty())
			{
				Log(LogLevel::Warning) << "Province " << province.first << " has no culture set!";
				continue;
			}
			religiousCensus[province.second->getReligion()] += 1;
			culturalCensus[province.second->getCulture()] += 1;
		}
		if (country.second->getPrimaryCulture().empty())
		{
			auto max = get_max(culturalCensus);
			Log(LogLevel::Warning) << country.first << " overriding blank culture with: " << max.first;
			country.second->setPrimaryCulture(max.first);
		}
		if (country.second->getMajorityReligion().empty())
		{
			auto max = get_max(religiousCensus);
			Log(LogLevel::Info) << country.first << "'s majority religion is: " << max.first;
			country.second->setMajorityReligion(max.first);
		}
		if (country.second->getReligion().empty())
		{
			auto max = get_max(religiousCensus);
			Log(LogLevel::Warning) << country.first << " overriding blank religion with: " << max.first;
			country.second->setReligion(max.first);
		}
		if (country.second->getTechGroup().empty())
		{
			const auto& techMatch = cultureMapper.getTechGroup(country.second->getPrimaryCulture());
			if (techMatch)
			{
				Log(LogLevel::Warning) << country.first << " overriding blank tech group with: " << *techMatch;
				country.second->setTechGroup(*techMatch);
			}
			else
			{
				country.second->setTechGroup("western");
				Log(LogLevel::Warning) << country.first << " could not determine technological group, substituting western!";
			}
		}
		if (country.second->getGFX().empty())
		{
			const auto& gfxMatch = cultureMapper.getGFX(country.second->getPrimaryCulture());
			if (gfxMatch)
			{
				Log(LogLevel::Warning) << country.first << " overriding blank gfx with: " << *gfxMatch;
				country.second->setGFX(*gfxMatch);
			}
			else
			{
				country.second->setGFX("westerngfx");
				Log(LogLevel::Warning) << country.first << " could not determine GFX, substituting westerngfx!";
			}
		}
	}
}

void EU4::World::assignAllCountryReforms()
{
	for (const auto& country: countries)
	{
		if (!country.second->getTitle() || !country.second->getGovernmentReforms().empty())
			continue;
		country.second->assignReforms(regionMapper, religionMapper, cultureDefinitionsMapper);
	}
}

void EU4::World::generateNationalIdeasFromDynamicCultures(const CK3::Cultures& cultures)
{
	Log(LogLevel::Info) << "-> Creating new National Ideas";

	dynamicIdeasMapper = mappers::DynamicIdeasMapper(localizationMapper);

	for (auto& culture: cultures.getCultures() | std::views::values)
	{
		if (culture->isDynamic())
			dynamicNationalIdeas.emplace_back(NationalIdeas(culture, dynamicIdeasMapper));
	}

	Log(LogLevel::Info) << "<> Created " << dynamicNationalIdeas.size() << " National Ideas.";
}

void EU4::World::importAdvisers()
{
	Log(LogLevel::Info) << "-> Importing Advisers";
	auto counter = 0;
	for (const auto& country: countries)
	{
		country.second->initializeAdvisers(localizationMapper, religionMapper, cultureMapper);
		counter += static_cast<int>(country.second->getAdvisers().size());
	}
	Log(LogLevel::Info) << "<> Imported " << counter << " advisers.";
}

void EU4::World::resolvePersonalUnions()
{
	Log(LogLevel::Info) << "-> Resolving annexations and Personal Unions";
	auto annexCounter = 0;
	auto puCounter = 0;
	std::map<long long, std::map<std::string, std::shared_ptr<Country>>> holderTitles;
	std::map<long long, std::pair<std::string, std::shared_ptr<Country>>> holderPrimaryTitle;
	std::map<long long, std::shared_ptr<CK3::Character>> relevantHolders;

	// We're filling the registry first.
	for (const auto& country: countries)
	{
		if (!country.second->getTitle() || !country.second->getTitle()->second)
			continue;
		if (!country.second->getTitle()->second->getHolder() || !country.second->getTitle()->second->getHolder()->second)
			continue;
		// we have a holder.
		const auto& holder = *country.second->getTitle()->second->getHolder();
		holderTitles[holder.first].insert(country);
		relevantHolders.insert(holder);
		// Does he have a primary title? Of course he does. They all do, but some may have been dropped as provinceless.
		// Look for first one that has an EU4 tag attached.
		if (holder.second->getCharacterDomain() && !holder.second->getCharacterDomain()->getDomain().empty())
		{
			auto foundTag = false;
			for (const auto& title: holder.second->getCharacterDomain()->getDomain())
			{
				if (!title.second)
					continue; // this title is broken.
				if (title.second->getEU4Tag())
				{
					foundTag = true;
					holderPrimaryTitle[holder.first] = *title.second->getEU4Tag();
					break;
				}
			}
			if (!foundTag)
			{
				// ... mooooving on.
				// Log(LogLevel::Warning) << country.first << " holder " << holder.first << " has no EU4tags in domain? Odd?";
				// Commented out warning due to c_nf_ title holders with junk in their domain.
			}
		}
		else
		{
			Log(LogLevel::Warning) << country.first << " holder " << holder.first << " has nothing in domain. Great.";
		}
	}

	// Now let's see what we have.
	for (const auto& holderTitle: holderTitles)
	{
		if (holderTitle.second.size() <= 1)
			continue;

		// multiple crowns. What's our primary?
		auto primaryItr = holderPrimaryTitle.find(holderTitle.first);
		std::pair<std::string, std::shared_ptr<Country>> primaryTitle;
		if (primaryItr == holderPrimaryTitle.end() || primaryItr->second.second->getProvinces().empty())
		{
			// We need to find another primary title.
			auto foundPrimary = false;
			// First check if we can find PAP or FAP or some special flag
			for (const auto& title: holderTitle.second)
			{
				if (title.first == "PAP" || title.second->isHREEmperor() || title.second->isHREElector())
				{
					primaryTitle = title;
					foundPrimary = true;
					break;
				}
			}
			// If no popes or specials pick first one.
			if (!foundPrimary)
				for (const auto& title: holderTitle.second)
				{
					if (!title.second->getProvinces().empty())
					{
						primaryTitle = title;
						foundPrimary = true;
						break;
					}
				}
			if (!foundPrimary)
				continue; // no helping this fellow.
		}
		else
		{
			primaryTitle = primaryItr->second;

			// That's lovely, but is this the special snowflake THE POPE? Does he hold PAP/FAP as secondary?
			for (const auto& title: holderTitle.second)
			{
				if (title.first == "PAP")
				{
					primaryTitle = std::pair(title.first, title.second);
					break;
				}
			}
		}

		// religion
		auto heathen = false;
		if (!relevantHolders[holderTitle.first]->getFaith() ||
			 relevantHolders[holderTitle.first]->getFaith()->second->getReligion().second->getName() != "christianity_religion")
			heathen = true;

		// We now have a holder, his primary, and religion. Let's resolve multiple crowns.
		if (!heathen && primaryTitle.second->getGovernment() == "monarchy")
		{
			auto unionCount = 0;
			for (const auto& title: holderTitle.second)
			{
				if (title.first == primaryTitle.first)
					continue;
				if (title.second->getProvinces().empty())
					continue;
				// Craft a relation. Going up to a max of 3 unions.
				if (unionCount <= 2)
				{
					diplomacy.addAgreement(std::make_shared<Agreement>(primaryTitle.first, title.first, "union", primaryTitle.second->getConversionDate()));
					++unionCount;
					++puCounter;
				}
				else
				{
					// too many unions.
					primaryTitle.second->annexCountry(title);
					++annexCounter;
					if (primaryTitle.second->isHREEmperor() && emperorTag != primaryTitle.first)
						emperorTag = primaryTitle.first;
				}
			}
		}
		else
		{
			// heathens annex straight up.
			for (const auto& title: holderTitle.second)
			{
				if (title.first == primaryTitle.first)
					continue;
				if (title.second->getProvinces().empty())
					continue;
				// Yum.
				primaryTitle.second->annexCountry(title);
				++annexCounter;
				if (primaryTitle.first == "PAP")
					Log(LogLevel::Debug) << primaryTitle.first << " is annexing " << title.first;
				if (primaryTitle.second->isHREEmperor() && emperorTag != primaryTitle.first)
					emperorTag = primaryTitle.first;
			}
		}
	}
	Log(LogLevel::Info) << "<> Annexed " << annexCounter << " and PUed " << puCounter << " countries.";
}

void EU4::World::markCelestialTitle(const Configuration& theConfiguration,
	 const std::optional<std::pair<std::string, std::shared_ptr<CK3::Title>>>& celestialTitle)
{
	if (theConfiguration.getCelestial() == Configuration::CELESTIAL::DESTROY)
	{
		Log(LogLevel::Debug) << ">< No MoH Holder per configuration.";
		return;
	}

   if (celestialTitle.has_value())
   {
		Log(LogLevel::Debug) << "has value";
		if (!celestialTitle->first.empty())
		{
			Log(LogLevel::Debug) << "not empty";
			if (celestialTitle->second)
			{
				Log(LogLevel::Debug) << "has seconf";
				if (celestialTitle->second->getEU4Tag())
					Log(LogLevel::Debug) << "eu4tag";
			}
		}
   }

	if (celestialTitle.has_value() && !celestialTitle->first.empty() && celestialTitle->second && celestialTitle->second->getEU4Tag())
	{
		MoHTag = celestialTitle->second->getEU4Tag()->first;
		const auto& moh = countries.at(MoHTag);
		moh->overrideReforms("celestial_empire");
		Log(LogLevel::Info) << "<> Celestial Empire assigned to: " << MoHTag;
		return;
	}

	int targetProvince = 0;
	if (theConfiguration.getCelestial() == Configuration::CELESTIAL::BEIJING)
		targetProvince = 1816;
	else if (theConfiguration.getCelestial() == Configuration::CELESTIAL::NANJING)
		targetProvince = 1821;
	else if (theConfiguration.getCelestial() == Configuration::CELESTIAL::CANTON)
		targetProvince = 667;

	// Who holds this?

   if (!provinces.contains(targetProvince))
   {
		Log(LogLevel::Error) << "There's no province " << targetProvince << " in China? What?";
		return;
   }
	const auto& province = provinces.at(targetProvince);

   if (province->getOwner().empty())
   {
		Log(LogLevel::Error) << "China is uncolonized? What?";
		return;
	}
	MoHTag = province->getOwner();

   if (!countries.contains(MoHTag))
   {
		Log(LogLevel::Error) << "There's no " << MoHTag << " in countries? What? Why?";
		return;
   }

	const auto& moh = countries.at(MoHTag);
	moh->setGovernment("monarchy");
	moh->overrideReforms("celestial_empire");
	moh->setGovernmentRank(3);
	Log(LogLevel::Info) << "<> No default China, so Celestial Empire granted to: " << MoHTag;
}

void EU4::World::markHRETag(const Configuration& theConfiguration,
	 const std::pair<std::string, std::shared_ptr<CK3::Title>>& hreTitle,
	 const date& conversionDate,
	 date startDate,
	 Configuration::STARTDATE startDateOption)
{
	if (theConfiguration.getHRE() == Configuration::I_AM_HRE::NONE)
		return;
	if (hreTitle.first.empty())
		return;
	const auto hreTag = titleTagMapper.getTagForTitle(hreTitle.first);
	if (!hreTag)
	{
		Log(LogLevel::Warning) << "No known tag for CK3's HRE Title: " << hreTitle.first << "! Add it to tag_mappings.txt!";
		return;
	}
	actualHRETag = *hreTag;
	if (actualHRETag == "HLR")
		actualHRETag = "HRE";

	Log(LogLevel::Debug) << hreTitle.first << " into " << actualHRETag;
	if (!countries.contains(actualHRETag))
	{
		const auto newCountry = std::make_shared<Country>();
		newCountry->initializeFromTitle(actualHRETag,
			 hreTitle,
			 governmentsMapper,
			 religionMapper,
			 cultureMapper,
			 provinceMapper,
			 localizationMapper,
			 rulerPersonalitiesMapper,
			 conversionDate,
			 startDate,
			 startDateOption,
			 false);
		countries.emplace(actualHRETag, newCountry);
	}

	Log(LogLevel::Info) << "<> Marked " << actualHRETag << " as HRE tag.";
}

void EU4::World::distributeHRESubtitles(const Configuration& theConfiguration)
{
	if (theConfiguration.getHRE() == Configuration::I_AM_HRE::NONE)
		return;
	Log(LogLevel::Info) << "-> Locating Emperor";
	// Emperor may or may not be set.
	for (const auto& country: countries)
		if (country.second->isHREEmperor())
		{
			emperorTag = country.first;
			Log(LogLevel::Info) << "<> Emperor is " << emperorTag << " (" << country.second->getTitle()->second->getName() << ", "
									  << country.second->getProvinces().size() << " provinces)";
			break;
		}
	if (!emperorTag.empty())
	{
		setFreeCities();
		setElectors();
	}
	else
		Log(LogLevel::Info) << "<> Emperor could not be found, no HRE mechanics.";
}

void EU4::World::setElectors()
{
	Log(LogLevel::Info) << "-> Setting Electors";
	std::vector<std::pair<double, std::shared_ptr<Country>>> bishops; // piety-tag
	std::vector<std::pair<int, std::shared_ptr<Country>>> duchies;		// dev-tag
	std::vector<std::pair<int, std::shared_ptr<Country>>> republics;	// dev-tag
	std::vector<std::shared_ptr<Country>> electors;
	int electorBishops = 0;
	int electorRepublics = 0;
	int electorDuchies = 0;

	// We need to be careful about papacy and orthodox holders
	for (const auto& country: countries)
	{
		if (country.second->isinHRE())
		{
			if (country.second->getProvinces().empty())
				continue;
			const auto& holder = country.second->getTitle()->second->getHolder();
			if (country.first == "PAP" || holder->second->getCharacterDomain()->getDomain()[0].second->getName() == "k_orthodox")
			{
				// override to always be elector
				electors.emplace_back(country.second);
				electorBishops++;
				continue;
			}
			// Let's shove all hre members into appropriate categories.
			if (country.second->getGovernment() == "theocracy")
			{
				if (country.second->getTitle()->second->isElectorate())
				{
					electorBishops++;
					electors.emplace_back(country.second);
				}
				else
				{
					bishops.emplace_back(std::pair(lround(holder->second->getPiety()), country.second));
				}
			}
			else if (country.second->getGovernment() == "monarchy")
			{
				if (country.second->getTitle()->second->isElectorate())
				{
					electorDuchies++;
					electors.emplace_back(country.second);
				}
				else
				{
					// skip juniors.
					if (diplomacy.isCountryJunior(country.first))
						continue;
					duchies.emplace_back(std::pair(country.second->getDevelopment(), country.second));
				}
			}
			else if (country.second->getGovernment() == "republic")
			{
				// No free cities, thank you!
				if (country.second->getGovernmentReforms().count("free_city"))
					continue;
				if (country.second->getTitle()->second->isElectorate())
				{
					electorRepublics++;
					electors.emplace_back(country.second);
				}
				else
				{
					republics.emplace_back(std::pair(country.second->getDevelopment(), country.second));
				}
			} // skipping tribal and similar.
		}
	}

	std::sort(bishops.rbegin(), bishops.rend());
	std::sort(duchies.rbegin(), duchies.rend());
	std::sort(republics.rbegin(), republics.rend());

	for (const auto& bishop: bishops)
	{
		if (electors.size() >= 7 || electorBishops >= 3)
			break;
		electors.emplace_back(bishop.second);
		electorBishops++;
	}
	for (const auto& republic: republics)
	{
		if (electors.size() >= 7 || electorBishops + electorRepublics >= 3)
			break;
		electors.emplace_back(republic.second);
		electorRepublics++;
	}
	for (const auto& duchy: duchies)
	{
		if (electors.size() >= 7)
			break;
		electors.emplace_back(duchy.second);
	}

	for (const auto& elector: electors)
	{
		elector->setElector();
		Log(LogLevel::Info) << "\t- Electorate set: " << elector->getTag() << " (from " << elector->getTitle()->second->getName() << ")";
	}
	Log(LogLevel::Info) << "<> There are " << electors.size() << " electors recognized.";
}

void EU4::World::setFreeCities()
{
	Log(LogLevel::Info) << "-> Setting Free Cities";
	// How many free cities do we already have?
	auto freeCityNum = 0;
	for (const auto& country: countries)
	{
		if (country.second->isinHRE() && country.second->getGovernment() == "republic" && country.second->getProvinces().size() == 1 &&
			 !country.second->getTitle()->second->getGeneratedLiege() && !country.second->getTitle()->second->isElectorate() && freeCityNum < 12)
		{
			country.second->overrideReforms("free_city");
			country.second->setMercantilism(25);
			++freeCityNum;
		}
	}
	// Can we turn some minors into free cities?
	if (freeCityNum < 12)
	{
		for (const auto& country: countries)
		{
			if (country.second->isinHRE() && country.second->getGovernment() != "republic" && !country.second->isHREEmperor() &&
				 country.second->getGovernmentReforms().empty() && country.second->getProvinces().size() == 1 && !diplomacy.isCountryJunior(country.first) &&
				 !country.second->getTitle()->second->getGeneratedLiege() && !country.second->getTitle()->second->isElectorate() && freeCityNum < 12)
			{
				if (country.first == "HAB")
					continue; // For Iohannes who is sensitive about Austria.
				// GovernmentReforms being empty ensures we're not converting special governments and targeted tags into free cities.
				country.second->overrideReforms("free_city");
				country.second->setGovernment("republic");
				country.second->setMercantilism(25);
				++freeCityNum;
			}
		}
	}
	Log(LogLevel::Info) << "<> There are " << freeCityNum << " free cities.";
}

void EU4::World::distributeClaims(const Configuration& theConfiguration)
{
	if (theConfiguration.getDejure() == Configuration::DEJURE::DISABLED)
	{
		Log(LogLevel::Info) << ">< Permaclaim distribution disabled by configuration.";
		return;
	}

	Log(LogLevel::Info) << "-- Distributing DeJure Claims";
	auto counter = 0;
	std::map<std::string, std::set<std::string>> claimsRegister; // ck3 county name, eu4 tag claims

	// Mapping all countries with all their claims.
	for (const auto& country: countries)
	{
		if (!country.second->getTitle())
			continue;
		if (country.second->getProvinces().empty())
			continue; // No claims for dead nations.
		for (const auto& DJcounty: country.second->getTitle()->second->getOwnedDJCounties())
		{
			claimsRegister[DJcounty.first].insert(country.first);
		}
	}

	// And then rolling through provinces and applying those claims.
	for (const auto& province: provinces)
	{
		if (!province.second->getSourceProvince())
			continue;
		const auto& claimItr = claimsRegister.find(province.second->getSourceProvince()->getName());
		if (claimItr == claimsRegister.end())
			continue;
		for (const auto& tag: claimItr->second)
		{
			if (tag == province.second->getOwner())
				continue;
			// since de jure claims are based on DE JURE land, we're adding it even for PU or vassal land.
			province.second->addPermanentClaim(tag);
			counter++;
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " claims have been distributed.";
}

void EU4::World::distributeDeadCores()
{
	Log(LogLevel::Info) << "-- Distributing Dead Cores";
	auto counter = 0;

	// Make a register of dead country tags we are allowed to use
	std::set<std::string> deadTags;

	for (const auto& country: countries)
		if (country.second->getProvinces().empty())
			deadTags.insert(country.first);

	// Then, for all provinces, see if their culture has a primary nation, and if we're allowed to insert it.
	for (const auto& province: provinces)
	{
		// Don't touch ROTW
		if (!province.second->getSourceProvince())
			continue;
		// Don't touch stuff we left empty
		if (province.second->getOwner().empty())
			continue;

		const auto& primaryTag = primaryTagMapper.getPrimaryTagForCulture(province.second->getCulture());
		if (primaryTag)
		{
			if (deadTags.count(*primaryTag))
			{
				// We're ok to use this.
				province.second->addCore(*primaryTag);
				++counter;
			}
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " dead cores have been distributed.";
}

void EU4::World::distributeForts()
{
	Log(LogLevel::Info) << "-- Building Forts";

	// We're doing this only for our new countries. We haven't deleted forts in ROTW.
	// Countries at 4+ provinces get a capital fort. 8+ countries get one fort per area where they own 3+ provinces.

	auto counterCapital = 0;
	auto counterOther = 0;

	for (const auto& country: countries)
	{
		if (!country.second->getTitle())
			continue;
		if (country.second->getProvinces().size() < 4)
			continue; // To small to afford forts.
		if (!country.second->getCapitalID())
			continue; // Not dealing with broken countries, thank you.
		if (!country.second->getProvinces().count(country.second->getCapitalID()))
		{
			if (country.first != "PAP")
				Log(LogLevel::Warning) << country.first << " has capital province set to " << country.second->getCapitalID() << " but doesn't own it?";
			continue; // this should have been fixed earlier by verifyCapitals! Well... Except for pope.
		}

		const auto& capitalAreaName = regionMapper->getParentAreaName(country.second->getCapitalID());
		if (!capitalAreaName)
			continue; // uh-huh

		const auto& capitalProvince = country.second->getProvinces().find(country.second->getCapitalID());
		capitalProvince->second->buildFort();
		counterCapital++;

		if (country.second->getProvinces().size() < 8)
			continue; // Too small for more forts.

		std::set<std::string> builtAreas = {*capitalAreaName};
		// Now it gets serious. We need a list of areas with 3+ provinces.
		std::map<std::string, int> elegibleAreas;

		for (const auto& province: country.second->getProvinces())
		{
			const auto& areaName = regionMapper->getParentAreaName(province.first);
			if (!areaName)
				continue;
			elegibleAreas[*areaName]++;
		}

		// And we put a fort in every third?
		auto counter = 0;
		for (const auto& province: country.second->getProvinces())
		{
			const auto& areaName = regionMapper->getParentAreaName(province.first);
			if (!areaName)
				continue;
			if (builtAreas.count(*areaName))
				continue;
			if (elegibleAreas[*areaName] <= 2)
				continue;
			if (counter < 2)
			{
				++counter;
				continue;
			}
			counter = 0;

			province.second->buildFort();
			counterOther++;
			builtAreas.insert(*areaName);
		}
	}

	Log(LogLevel::Info) << "<> " << counterCapital << " capital forts and " << counterOther << " other forts have been built.";
}

void EU4::World::fixTengri()
{
	// We need to convert all unmapped EU4 Tengri provinces to Old Tengri (Unreformed)
	Log(LogLevel::Info) << "-- Checking for Reformed Tengri";
	for (const auto& province: provinces)
	{
		if (!province.second->getSourceProvince())
		{
			if (province.second->getReligion() == "tengri_pagan_reformed")
			{
				province.second->setReligion("tengri_pagan");
			}
		}
	}
	for (const auto& country: countries)
	{
		if (!country.second->getTitle())
		{
			if (country.second->getReligion() == "tengri_pagan_reformed")
			{
				country.second->setReligion("tengri_pagan");
			}
		}
	}
	Log(LogLevel::Info) << ">< Tengri successfully unreformed";
}

void EU4::World::fixDuplicateNames()
{
	Log(LogLevel::Info) << "-- Renaming Duplicate Countries";
	auto counter = 0;

	// Iterate through countries and assign their names as keywords. If a name appears more than once, add it to list of pointers
	std::map<const std::string, std::vector<std::shared_ptr<Country>>> nameMap;
	for (const auto& country: countries)
	{
		if (country.second->getProvinces().empty())
			continue;
		const auto& countryLocs = country.second->getLocalizations();
		if (countryLocs.empty())
			continue;
		if (countryLocs.find(country.first) == countryLocs.end())
			continue;
		if (countryLocs.find(country.first)->second.english.empty())
			continue;
		nameMap[countryLocs.find(country.first)->second.english].emplace_back(country.second);
	}

	// Reorder countries in list by development (highest -> lowest)
	for (auto& countryBatch: nameMap)
		std::sort(countryBatch.second.begin(), countryBatch.second.end(), [](auto a, auto b) {
			return a->getDevelopment() > b->getDevelopment();
		});

	// Now we iterate through all batches and sort out the names.
	for (const auto& countryBatch: nameMap)
	{
		// Bail for singletons
		if (countryBatch.second.size() <= 1)
			continue;

		// This is the locblock we're operating on. Should be same for all countries in this batch.
		auto currentBlock = countryBatch.second[0]->getLocalizations().find(countryBatch.second[0]->getTag())->second;

		// Is this a dynastyname? These will follow a bit different rules.
		auto dynastyName = countryBatch.second[0]->getHasDynastyName();

		// Clear out any "Greater" from name.
		auto greaterdropped = false;
		if (currentBlock.english.find("Greater ") != std::string::npos)
		{
			currentBlock.english = currentBlock.english.erase(0, 8); // -Greater
			currentBlock.spanish = currentBlock.spanish.erase(0, 5); // -Gran
			currentBlock.french = currentBlock.french.erase(0, 7);	// -Grande
			currentBlock.german = currentBlock.german.erase(0, 6);	// -Gross
			greaterdropped = true;
		}

		// Ditto for "Lesser"
		auto lesserdropped = false;
		if (currentBlock.english.find("Lesser ") != std::string::npos)
		{
			currentBlock.english = currentBlock.english.erase(0, 7); // -Lesser
			currentBlock.spanish = currentBlock.spanish.erase(0, 8); // -Pequeca
			currentBlock.french = currentBlock.french.erase(0, 7);	// -Petite
			currentBlock.german = currentBlock.german.erase(0, 6);	// -Klein
			lesserdropped = true;
		}

		// and now let's get to work.

		for (auto i = 0; i < static_cast<int>(countryBatch.second.size()); i++)
		{
			mappers::LocBlock newBlock;
			const auto& actualCountry = countryBatch.second[i];
			const auto& actualTag = actualCountry->getTag();
			const auto& countryLocs = actualCountry->getLocalizations();

			// First we deal with dynastynames.
			if (dynastyName)
			{
				switch (i)
				{
					case 0:
						// First country. Nothing changes (Ottomans).
						break;
					case 5:
						// For the fifth, we do Secret Ottomans.
						newBlock.english = "Secret " + currentBlock.english;
						newBlock.spanish = currentBlock.spanish + " Secreta";
						newBlock.french = currentBlock.french + " Secrete";
						newBlock.german = "Geheimnis " + currentBlock.german;
						actualCountry->setLocalizations(newBlock);
						break;
					default:
						// Otherwise we're trying to fuse TAG_ADJ and a canonical name for the country. (Ottoman Austria)
						if (countryLocs.count("canonical") && countryLocs.find(actualTag + "_ADJ") != countryLocs.end() &&
							 !countryLocs.find(actualTag + "_ADJ")->second.english.empty())
						{
							// We have the ingredients and can create the name.
							const auto& canonicalBlock = countryLocs.find("canonical")->second;
							const auto& adjBlock = countryLocs.find(actualTag + "_ADJ")->second;
							newBlock.english = adjBlock.english + " " + canonicalBlock.english;
							newBlock.spanish = adjBlock.spanish + " " + canonicalBlock.spanish;
							newBlock.german = adjBlock.german + " " + canonicalBlock.german;
							newBlock.french = adjBlock.french + " " + canonicalBlock.french;
							actualCountry->setLocalizations(newBlock);
							break;
						}
						// If we at least have canonical, we can bail from dynasty names. (just Austria)
						if (countryLocs.count("canonical"))
						{
							newBlock = countryLocs.find("canonical")->second;
							actualCountry->setLocalizations(newBlock);
							break;
						}
						// Else crap. We don't have a canonical name for these fellows. Leave as is.
						break;
				}
			}

			else // These would be regular, non-dynastyname countries.
			{
				switch (i)
				{
					case 0:
						// This is the Greater one, unless it already was the greater one. In that case we diversify.
						if (!greaterdropped)
						{
							newBlock.english = "Greater " + currentBlock.english;
							newBlock.spanish = "Gran " + currentBlock.spanish;
							newBlock.french = "Grande " + currentBlock.french;
							newBlock.german = "Gross " + currentBlock.german;
						}
						else
						{
							newBlock.english = currentBlock.english + " Major";
							newBlock.spanish = currentBlock.spanish + " Maior";
							newBlock.french = currentBlock.french + " Majeur";
							newBlock.german = currentBlock.german + " Maior";
						}
						actualCountry->setLocalizations(newBlock);
						break;
					case 1:
						// This is Lesser, unless it's Minor.
						if (!lesserdropped)
						{
							newBlock.english = "Lesser " + currentBlock.english;
							newBlock.spanish = "Pequeca " + currentBlock.spanish;
							newBlock.french = "Petite " + currentBlock.french;
							newBlock.german = "Klein " + currentBlock.german;
						}
						else
						{
							newBlock.english = currentBlock.english + " Minor";
							newBlock.spanish = currentBlock.spanish + " Minor";
							newBlock.french = currentBlock.french + " Mineur";
							newBlock.german = currentBlock.german + " Minor";
						}
						actualCountry->setLocalizations(newBlock);
						break;
					case 2:
						// Hello Hither.
						newBlock.english = "Hither " + currentBlock.english;
						newBlock.spanish = "Hither " + currentBlock.english;
						newBlock.french = "Hither " + currentBlock.english;
						newBlock.german = "Hither " + currentBlock.english;
						actualCountry->setLocalizations(newBlock);
						break;
					case 3:
						// Hello Further.
						newBlock.english = "Further " + currentBlock.english;
						newBlock.spanish = "Lejos " + currentBlock.spanish;
						newBlock.french = "Plus " + currentBlock.french;
						newBlock.german = "Ferner " + currentBlock.german;
						actualCountry->setLocalizations(newBlock);
						break;
					case 4:
						// This one's easy.
						newBlock.english = "Secret " + currentBlock.english;
						newBlock.spanish = currentBlock.spanish + " Secreta";
						newBlock.french = currentBlock.french + " Secrete";
						newBlock.german = "Geheimnis " + currentBlock.german;
						actualCountry->setLocalizations(newBlock);
						break;
					default:
						// Out of ideas. 6 Polands? Don't care any more.
						break;
				}
			}
			counter++;
		}
	}

	Log(LogLevel::Info) << "<> " << counter << " names unduplicated.";
}

void EU4::World::siberianQuestion(const Configuration& theConfiguration)
{
	Log(LogLevel::Info) << "-- Burning Siberia";
	if (theConfiguration.getSiberia() == Configuration::SIBERIA::LEAVE_SIBERIA)
	{
		Log(LogLevel::Info) << ">< Leaving Siberia alone due to Configuration.";
		return;
	}

	auto counter = 0;
	std::set<std::string> exceptions = {"CHU", "HOD", "CHV", "KMC"}; // kamchatkans.

	// We're deleting all tags with:
	// * capital is in Siberia
	// * nomad or tribal level
	// * no more than 5 provinces / option
	for (const auto& country: countries)
	{
		if (country.second->getGovernment() != "nomad" && country.second->getGovernment() != "tribal")
			continue;
		if (country.second->getProvinces().empty())
			continue;
		if (!country.second->getCapitalID())
			continue;
		const auto& region = regionMapper->getParentRegionName(country.second->getCapitalID());
		if (!region)
			continue;
		if (region != "west_siberia_region" && region != "east_siberia_region")
			continue;
		if (theConfiguration.getSiberia() == Configuration::SIBERIA::SMALL_SIBERIA && country.second->getProvinces().size() >= 5)
			continue;
		if (exceptions.count(country.second->getTag()))
			continue; // don't touch far-east vanilla tribes.

		// All checks done. Let's get deleting.
		for (const auto& province: country.second->getProvinces())
		{
			province.second->sterilize();
		}
		country.second->clearProvinces();
		country.second->clearExcommunicated();
		diplomacy.deleteAgreementsWithTag(country.first);
		++counter;
	}
	Log(LogLevel::Info) << "<> " << counter << " Siberian countries have been removed";
}

void EU4::World::africanQuestion()
{
	Log(LogLevel::Info) << "-- Checking Africa";

	// If a country has a province on both ends of the pass, or if they are in HRE, it stays, otherwise the pass is getting cleared

	for (const auto& pass: africanPassesMapper.getPasses())
	{
		std::vector<int> allProvinces = pass.getEndA();
		allProvinces.insert(allProvinces.end(), pass.getEndB().begin(), pass.getEndB().end());
		allProvinces.insert(allProvinces.end(), pass.getSterilizeProvinces().begin(), pass.getSterilizeProvinces().end());

		// first check the sanity of all provinces.
		auto sanity = true;
		for (const auto& provinceID: allProvinces)
		{
			if (provinces.find(provinceID) == provinces.end())
			{
				Log(LogLevel::Warning) << "Province " << provinceID << " in pass " << pass.getName() << " not found! Skipping!";
				sanity = false;
				break;
			}
			if (!provinces.find(provinceID)->second)
			{
				Log(LogLevel::Warning) << "Province " << provinceID << " in pass " << pass.getName() << " is dangling! Skipping!";
				sanity = false;
				break;
			}
		}
		if (!sanity)
			continue;

		// check owners of pass ends.
		std::set<std::string> ownersA;
		std::set<std::string> ownersB;
		bool hasStart = false;
		bool hasEnd = false;

		for (const auto& provinceID: pass.getEndA())
		{
			if (!provinces.find(provinceID)->second->getOwner().empty())
				ownersA.insert(provinces.find(provinceID)->second->getOwner());
		}
		for (const auto& provinceID: pass.getEndB())
		{
			if (!provinces.find(provinceID)->second->getOwner().empty())
				ownersB.insert(provinces.find(provinceID)->second->getOwner());
		}
		for (const auto& owner: ownersA)
		{
			if (ownersB.contains(owner))
			{
				hasStart = true;
				break;
			}
		}
		if (hasStart)
		{
			Log(LogLevel::Info) << "<- " << pass.getName() << " Pass colonized.";
			continue; // all under same owner.
		}

		// check HRE
		hasStart = false;

		for (const auto& provinceID: pass.getEndA())
		{
			const auto& country = countries.find(provinces.find(provinceID)->second->getOwner());
			if (country != countries.end() && country->second && country->second->isinHRE())
				hasStart = true;
		}
		for (const auto& provinceID: pass.getEndB())
		{
			const auto& country = countries.find(provinces.find(provinceID)->second->getOwner());
			if (country != countries.end() && country->second && country->second->isinHRE())
				hasEnd = true;
		}
		if (hasStart && hasEnd)
		{
			Log(LogLevel::Info) << "<- " << pass.getName() << " Pass HREified.";
			continue; // all provinces in hre
		}

		// sterilize
		for (const auto& provinceID: pass.getSterilizeProvinces())
			provinces.find(provinceID)->second->sterilize();

		Log(LogLevel::Info) << "<> " << pass.getName() << " Pass Sterilized.";
	}
}

void EU4::World::mongolianQuestion()
{
	Log(LogLevel::Info) << "-> Resolving the Mongolian Question";

	// Case 1. There was an e_mongolia or k_otuken that mapped to KHA. KHA now extends into CK3-scope.
	// Solution: Nothing needs to be done.
	const auto& kha = countries.at("KHA");

	if (kha->getTitle())
	{
		Log(LogLevel::Info) << "<> Mongolian Question resolved by marriage.";
		return;
	}

	// Case 2. There is noone that mapped to KHA. The Mongols exist, but they took over some other title and mapped to some other tag.
	// Solution: Find them. Check if they own EU4 provinces that Mongolia borders on the other side. If yes, rename them to KHA.

	// Step 1. "Find Them." Easy, they said. It should be TRIVIAL! Especially if they own k_england as primary title and the display name is in chinese.

	const auto& mongolLocBlock =
		 localizationMapper.getLocBlockForKey("mongol_collective_noun"); // this is "Mongols", not "The Mongols" we have as display name. Now we go hunting.
	if (!mongolLocBlock)
	{
		Log(LogLevel::Error) << "Game locs don't have mongol_collective_noun. Ask your service provider for a fresh installation.";
		return;
	}

	const auto& acutalMongol = localizationMapper.getLocBlockForKey("mongol_clothing_gfx"); // "Mongolian". Don't ask.
	if (!acutalMongol)
	{
		Log(LogLevel::Error) << "Game locs don't have mongol_clothing_gfx. Ask your service provider for a fresh installation.";
		return;
	}

	const auto& mongolNames = mongolLocBlock->getAllNames();

	std::vector<std::shared_ptr<Country>> potentialCandidates;
	std::string correctLanguage;

	for (const auto& country: countries | std::views::values)
	{
		if (!country->getLocalizations().contains(country->getTag()))
			continue;

		auto countryNames = country->getLocalizations().at(country->getTag()).getAllNames();
		// does any ~~match~~  CONTAIN  any of the mongols locs? Jesus wept.

		for (const auto& [monLanguage, mongolName]: mongolNames)
		{
			for (const auto& countryName: countryNames | std::views::values)
			{
				if (countryName.find(mongolName) != std::string::npos)
				{
					// See? Potential! Because me may have just hit any of the indep mongol tribes, not the actual host.
					potentialCandidates.emplace_back(country);
					correctLanguage = monLanguage;
				}
			}
		}
	}

	if (potentialCandidates.empty())
	{
		Log(LogLevel::Info) << "<> Mongolian Question resolved by attrition.";
		return;
	}

	// Now see who has the shortest name. Not joking. Yes, I know.

	std::shared_ptr<Country> shortestNameContestWinner;
	size_t length = 9999;
	for (const auto& candidate: potentialCandidates)
	{
		if (candidate->getLocalizations().at(candidate->getTag()).getAllNames().at(correctLanguage).size() < length)
		{
			shortestNameContestWinner = candidate;
			length = candidate->getLocalizations().at(candidate->getTag()).getAllNames().at(correctLanguage).size();
		}
	}

	// Are we even a valid target? Mongols in France are not KHA unless they border actual KHA.

	std::set<int> relevantProvinces = {1056, 1057, 4220, 2116, 4221, 4677, 4676, 709}; // these are CK3-scope border provinces towards off-scope KHA.

	bool borderCheck = false;
	for (const auto& provinceID: shortestNameContestWinner->getProvinces() | std::views::keys)
	{
		if (relevantProvinces.contains(provinceID))
		{
			borderCheck = true;
			break;
		}
	}

	if (!borderCheck)
	{
		Log(LogLevel::Info) << "<> Mongolian Question resolved by tribal migration.";
		return;
	}

	// Now the trivial task of reassigning the entire identity of the country to KHA. Easier for KHA to become mongols.
	kha->subsumeCountry(shortestNameContestWinner);

	// KHA_ADJ will now be buryatian or french or whatever nonsense. Need to fix it.

	auto locs = kha->getLocalizations();

	if (locs.contains("KHA_ADJ"))
		locs.at("KHA_ADJ") = *acutalMongol;
	else
		locs.emplace("KHA_ADJ", *acutalMongol);
	kha->setLocalizations(locs);

	Log(LogLevel::Info) << "<> Mongolian Question resolved by Jurchen invasion of " << shortestNameContestWinner->getTag() << " - "
							  << shortestNameContestWinner->getLocalizations().at(shortestNameContestWinner->getTag()).english << ".";
}
