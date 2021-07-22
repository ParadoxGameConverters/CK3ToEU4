#include "../../CK3World/World.h"
#include "../../Configuration/Configuration.h"
#include "../EU4World.h"
#include "../Province/EU4Province.h"
#include "CommonFunctions.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

void EU4::World::output(const commonItems::ConverterVersion& converterVersion, const Configuration& theConfiguration, const CK3::World& sourceWorld) const
{
	const auto invasion = theConfiguration.getSunset() == Configuration::SUNSET::ACTIVE;
	const auto dynamicInstitutions = theConfiguration.getDynamicInstitutions() == Configuration::INSTITUTIONS::DYNAMIC;
	const date conversionDate = sourceWorld.getConversionDate();
	Log(LogLevel::Info) << "<- Creating Output Folder";

	commonItems::TryCreateFolder("output");
	if (commonItems::DoesFolderExist("output/" + theConfiguration.getOutputName()))
	{
		Log(LogLevel::Info) << "<< Deleting existing mod folder.";
		commonItems::DeleteFolder("output/" + theConfiguration.getOutputName());
	}
	Log(LogLevel::Progress) << "80 %";

	Log(LogLevel::Info) << "<- Copying Mod Template";
	commonItems::CopyFolder("blankMod/output", "output/output");
	Log(LogLevel::Progress) << "81 %";

	Log(LogLevel::Info) << "<- Moving Mod Template >> " << theConfiguration.getOutputName();
	commonItems::RenameFolder("output/output", "output/" + theConfiguration.getOutputName());
	Log(LogLevel::Progress) << "82 %";

	commonItems::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/");
	commonItems::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/countries/");
	commonItems::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/advisors/");
	commonItems::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/provinces/");
	commonItems::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/diplomacy/");
	commonItems::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/common/");
	commonItems::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/common/countries/");
	commonItems::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/common/country_tags/");
	commonItems::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/localisation/");
	Log(LogLevel::Progress) << "83 %";

	Log(LogLevel::Info) << "<- Crafting .mod File";
	createModFile(theConfiguration);
	Log(LogLevel::Progress) << "84 %";

	// Record converter version
	Log(LogLevel::Info) << "<- Writing version";
	outputVersion(converterVersion, theConfiguration);
	Log(LogLevel::Progress) << "85 %";

	// Output common\countries.txt
	Log(LogLevel::Info) << "<- Creating countries.txt";
	outputCommonCountriesFile(theConfiguration);
	Log(LogLevel::Progress) << "86 %";

	Log(LogLevel::Info) << "<- Writing Country Commons";
	outputCommonCountries(theConfiguration);
	Log(LogLevel::Progress) << "87 %";

	Log(LogLevel::Info) << "<- Writing Country Histories";
	outputHistoryCountries(theConfiguration);
	Log(LogLevel::Progress) << "88 %";

	Log(LogLevel::Info) << "<- Writing Religions";
	outputReligions(theConfiguration, religionMapper.getGeneratedReligions(), religionMapper.getReformedReligions());
	Log(LogLevel::Progress) << "89 %";

	if (invasion)
	{
		Log(LogLevel::Info) << "<- Writing Sunset Invasion Files";
		outputInvasionExtras(theConfiguration);
	}
	if (dynamicInstitutions)
	{
		Log(LogLevel::Info) << "<- Writing Dynamic Institution Files";
		outputDynamicInstitutions(theConfiguration);
	}

	Log(LogLevel::Info) << "<- Writing Advisers";
	outputAdvisers(theConfiguration);
	Log(LogLevel::Progress) << "90 %";

	Log(LogLevel::Info) << "<- Writing Provinces";
	outputHistoryProvinces(theConfiguration);
	Log(LogLevel::Progress) << "91 %";

	Log(LogLevel::Info) << "<- Writing Localization";
	outputLocalization(theConfiguration, invasion);
	Log(LogLevel::Progress) << "92 %";

	Log(LogLevel::Info) << "<- Writing Emperor";
	outputEmperor(theConfiguration, conversionDate);
	Log(LogLevel::Progress) << "93 %";

	Log(LogLevel::Info) << "<- Writing Diplomacy";
	outputDiplomacy(theConfiguration, diplomacy.getAgreements(), invasion);
	Log(LogLevel::Progress) << "94 %";

	Log(LogLevel::Info) << "<- Copying Flags";
	outputFlags(theConfiguration);
	Log(LogLevel::Progress) << "95 %";

	Log(LogLevel::Info) << "<- Writing Religion Icons";
	outputReligionIcons(theConfiguration, religionMapper.getGeneratedReligions(), sourceWorld.getMods());
	Log(LogLevel::Progress) << "96 %";

	Log(LogLevel::Info) << "<- Replacing Bookmark";
	outputBookmark(theConfiguration, conversionDate);
	Log(LogLevel::Progress) << "97 %";
}

void EU4::World::outputReligionIcons(const Configuration& theConfiguration, const std::vector<GeneratedReligion>& generatedReligions, const Mods& mods) const
{
	// edit the strips
	flagFoundry.extendReligionStrips(theConfiguration, generatedReligions, mods);

	// edit interface file, raw search/replace.
	const auto originalIcons = religionDefinitionMapper.getOriginalIconCount();
	auto currentIcons = originalIcons + religionMapper.getGeneratedReligions().size();
	if (!commonItems::DoesFileExist("output/" + theConfiguration.getOutputName() + "/interface/z_converter.gfx"))
		throw std::runtime_error("Cannot open output/" + theConfiguration.getOutputName() + "/interface/z_converter.gfx");

	std::ifstream sourceInterfaces("output/" + theConfiguration.getOutputName() + "/interface/z_converter.gfx");
	std::stringstream interfaceStream;
	interfaceStream << sourceInterfaces.rdbuf();
	sourceInterfaces.close();

	auto interfaceData = interfaceStream.str();
	auto swap = std::to_string(originalIcons);
	auto with = std::to_string(currentIcons);
	auto pos = interfaceData.find(swap);
	while (pos != std::string::npos)
	{
		interfaceData.replace(pos, swap.size(), with);
		// Get the next occurrence from the current position
		pos = interfaceData.find(swap, pos + with.size());
	}

	std::ofstream interFaceDump("output/" + theConfiguration.getOutputName() + "/interface/z_converter.gfx");
	interFaceDump << interfaceData;
	interFaceDump.close();
}


void EU4::World::outputReligions(const Configuration& theConfiguration,
	 const std::vector<GeneratedReligion>& generatedReligions,
	 const std::vector<std::string>& reformedReligions) const
{
	// Reformed Religion global flag output
	std::ofstream globalFlagFile("output/" + theConfiguration.getOutputName() + "/common/on_actions/ZZZ_replaced_on_startup.txt");
	// Basegame stuff first
	globalFlagFile << "on_startup = {\n\t"
						<< "emperor = {\n\t\t"
						<< "save_global_event_target_as = Emperor\n\t"
						<< "}\n\t"
						<< "if = {\n\t\t"
						<< "limit = { is_emperor_of_china = yes }\n\t\t"
						<< "save_global_event_target_as = EmperorOfChina\n\t"
						<< "}\n\t"
						<< "events = {\n\t\t"
						<< "muslim_school_events.20 #Pick School #flavor_fra .15000 #Make the French Revolution happen if starting in 1789\n\t"
						<< "}\n\t"
						<< "if = {\n\t\t"
						<< "limit = { has_dlc = \"Cradle of Civilization\" has_country_modifier = tur_janissary }\n\t\t"
						<< "remove_country_modifier = tur_janissary\n\t"
						<< "}\n\t"
						<< "initialize_schools_effect = yes #This is only used here but is used for readability of on_action file.\n\n\t"
						<< "if = {\n\t\t"
						<< "limit = { NOT = { has_country_flag = new_flavour_bav_13_has_been_triggered } OR = { tag = UBV tag = LBV tag = ING } }\n\t\t"
						<< "set_country_flag = new_flavour_bav_13_has_been_triggered country_event = { id = new_flavour_bav.13 days = 90 } #better not to allow "
							"almost instant DOWs\n\t"
						<< "}\n\t"
						<< "if = {\n\t\t"
						<< "limit = { tag = FRA started_in = 1789.7.14 NOT = { is_year = 1790 } NOT = { has_disaster = french_revolution } }\n\t\t"
						<< "add_disaster_progress = { value = 100 disaster = french_revolution }\n\t"
						<< "}\n\t";
	// Now our stuff
	globalFlagFile << "if={\n\t\tlimit = { NOT = { has_global_flag = ZZZ_enhanced_random_world } }";
	for (const auto& religion: reformedReligions)
	{
		globalFlagFile << "\n\t\tset_global_flag = ZZZ_enhanced_" << religion << "_reformed";
	}
	globalFlagFile << "\n\t}\n}";
	globalFlagFile.close();

	for (const auto& religion: generatedReligions)
	{
		// Main Religion File
		std::ofstream religionFile("output/" + theConfiguration.getOutputName() + "/common/religions/99_" + religion.name + "-from-" + religion.parent + ".txt");
		religionFile << religion;
		religionFile.close();
		// Religious Rebels
		std::ofstream religionRebelFile(
			 "output/" + theConfiguration.getOutputName() + "/common/rebel_types/99_rebel_" + religion.name + "-from-" + religion.parent + ".txt");
		religion.outputRebels(religionRebelFile);
		religionRebelFile.close();
		// Religion Sounds
		std::ofstream religionSoundFile(
			 "output/" + theConfiguration.getOutputName() + "/sound/ZZZ_converted_" + religion.name + "-from-" + religion.parent + "sounds.asset");
		religion.outputSounds(religionSoundFile);
		religionSoundFile.close();
	}
}

void EU4::World::outputBookmark(const Configuration& theConfiguration, date conversionDate) const
{
	if (theConfiguration.getStartDate() == Configuration::STARTDATE::CK)
	{
		// fix the dynamic bookmark in defines
		if (!commonItems::DoesFileExist("output/" + theConfiguration.getOutputName() + "/common/defines/00_converter_defines.lua"))
			throw std::runtime_error("Can not find output/" + theConfiguration.getOutputName() + "/common/defines/00_converter_defines.lua!");
		std::ofstream defines("output/" + theConfiguration.getOutputName() + "/common/defines/00_converter_defines.lua");
		defines << "-- Defines modified by the converter\n\n";
		defines << "\nNDefines.NGame.START_DATE = \"" << conversionDate << "\"\n";
		defines.close();

		// fix the dynamic bookmark in bookmark file
		if (!commonItems::DoesFileExist("output/" + theConfiguration.getOutputName() + "/common/bookmarks/converter_bookmark.txt"))
			throw std::runtime_error("Can not find output/" + theConfiguration.getOutputName() + "/common/bookmarks/converter_bookmark.txt!");

		std::string startDate = "<CONVERSIONDATE>";
		std::ostringstream incomingBookmarks;

		std::ifstream bookmarks_txt("output/" + theConfiguration.getOutputName() + "/common/bookmarks/converter_bookmark.txt");
		incomingBookmarks << bookmarks_txt.rdbuf();
		bookmarks_txt.close();
		auto strBookmarks = incomingBookmarks.str();
		auto pos2 = strBookmarks.find(startDate);
		strBookmarks.replace(pos2, startDate.length(), conversionDate.toString());
		std::ofstream out_bookmarks_txt("output/" + theConfiguration.getOutputName() + "/common/bookmarks/converter_bookmark.txt");
		out_bookmarks_txt << strBookmarks;
		out_bookmarks_txt.close();

		// And wipe regular one
		std::ifstream bookmarks1444_txt("output/" + theConfiguration.getOutputName() + "/common/bookmarks/converter_bookmark_1444.txt",
			 std::ofstream::out | std::ofstream::trunc);
		bookmarks1444_txt.close();
	}
	else
	{
		// Vanilla defines are fine, just wipe dynamic bookmark
		std::ifstream bookmarks_txt("output/" + theConfiguration.getOutputName() + "/common/bookmarks/converter_bookmark.txt",
			 std::ofstream::out | std::ofstream::trunc);
		bookmarks_txt.close();
	}
}

void EU4::World::createModFile(const Configuration& theConfiguration) const
{
	std::ofstream output("output/" + theConfiguration.getOutputName() + ".mod");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + theConfiguration.getOutputName() + ".mod");
	Log(LogLevel::Info) << "<< Writing to: "
							  << "output/" + theConfiguration.getOutputName() + ".mod";
	output << modFile;
	output.close();

	std::ofstream output2("output/" + theConfiguration.getOutputName() + "/descriptor.mod");
	if (!output2.is_open())
		throw std::runtime_error("Could not create " + theConfiguration.getOutputName() + "/descriptor.mod");
	Log(LogLevel::Info) << "<< Writing to: "
							  << "output/" + theConfiguration.getOutputName() + "/descriptor.mod";
	output2 << modFile;
	output2.close();
}

void EU4::World::outputVersion(const commonItems::ConverterVersion& converterVersion, const Configuration& theConfiguration) const
{
	std::ofstream output("output/" + theConfiguration.getOutputName() + "/ck3toeu4_version.txt");
	if (!output.is_open())
		throw std::runtime_error("Error writing version file! Is the output folder writable?");
	output << converterVersion;
	output.close();
}

void EU4::World::outputInvasionExtras(const Configuration& theConfiguration) const
{
	// Sunset Religions
	auto files = commonItems::GetAllFilesInFolder("configurables/sunset/common/religions/");
	for (const auto& file: files)
		commonItems::TryCopyFile("configurables/sunset/common/religions/" + file, "output/" + theConfiguration.getOutputName() + "/common/religions/" + file);
	// Sunset Ideas
	files = commonItems::GetAllFilesInFolder("configurables/sunset/common/ideas/");
	for (const auto& file: files)
		commonItems::TryCopyFile("configurables/sunset/common/ideas/" + file, "output/" + theConfiguration.getOutputName() + "/common/ideas/" + file);
	// Sunset Cultures
	files = commonItems::GetAllFilesInFolder("configurables/sunset/common/cultures/");
	for (const auto& file: files)
		commonItems::TryCopyFile("configurables/sunset/common/cultures/" + file, "output/" + theConfiguration.getOutputName() + "/common/cultures/" + file);
	// Sunset Decisions
	files = commonItems::GetAllFilesInFolder("configurables/sunset/decisions/");
	for (const auto& file: files)
		commonItems::TryCopyFile("configurables/sunset/decisions/" + file, "output/" + theConfiguration.getOutputName() + "/decisions/" + file);
}

void EU4::World::outputDynamicInstitutions(const Configuration& theConfiguration) const
{
	// Dynamic Institions
	auto files = commonItems::GetAllFilesInFolder("configurables/dynamicInstitutions/institutions/");
	commonItems::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/common/institutions/");
	for (const auto& file: files)
		commonItems::TryCopyFile("configurables/dynamicInstitutions/institutions/" + file,
			 "output/" + theConfiguration.getOutputName() + "/common/institutions/" + file);
	// Dynamic Ideas
	files = commonItems::GetAllFilesInFolder("configurables/dynamicInstitutions/ideas/");
	for (const auto& file: files)
		commonItems::TryCopyFile("configurables/dynamicInstitutions/ideas/" + file, "output/" + theConfiguration.getOutputName() + "/common/ideas/" + file);
}

void EU4::World::outputCommonCountriesFile(const Configuration& theConfiguration) const
{
	std::ofstream output("output/" + theConfiguration.getOutputName() + "/common/country_tags/00_countries.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create countries file!");
	output << "REB = \"countries/Rebels.txt\"\n\n"; // opening with rebels manually.

	for (const auto& country: countries)
	{
		if (specialCountryTags.count(country.first))
			continue; // Not outputting specials.
		if (country.first != "REB")
			output << country.first << " = \"" << country.second->getCommonCountryFile() << "\"\n";
	}
	output << "\n";
	output.close();
}

void EU4::World::outputCommonCountries(const Configuration& theConfiguration) const
{
	for (const auto& country: countries)
	{
		std::ofstream output("output/" + theConfiguration.getOutputName() + "/common/" + country.second->getCommonCountryFile());
		if (!output.is_open())
			throw std::runtime_error(
				 "Could not create country common file: output/" + theConfiguration.getOutputName() + "/common/" + country.second->getCommonCountryFile());
		country.second->outputCommons(output);
		output.close();
	}
}

void EU4::World::outputHistoryCountries(const Configuration& theConfiguration) const
{
	for (const auto& country: countries)
	{
		std::ofstream output("output/" + theConfiguration.getOutputName() + "/" + country.second->getHistoryCountryFile());
		if (!output.is_open())
			throw std::runtime_error(
				 "Could not create country history file: output/" + theConfiguration.getOutputName() + "/" + country.second->getHistoryCountryFile());
		output << *country.second;
		output.close();
	}
}

void EU4::World::outputAdvisers(const Configuration& theConfiguration) const
{
	std::ofstream output("output/" + theConfiguration.getOutputName() + "/history/advisors/00_converter_advisors.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + theConfiguration.getOutputName() + "/history/advisors/00_converter_advisors.txt");
	for (const auto& country: countries)
	{
		country.second->outputAdvisers(output);
	}
	output.close();
}

void EU4::World::outputHistoryProvinces(const Configuration& theConfiguration) const
{
	for (const auto& province: provinces)
	{
		std::ofstream output("output/" + theConfiguration.getOutputName() + "/" + province.second->getHistoryCountryFile());
		if (!output.is_open())
			throw std::runtime_error(
				 "Could not create country history file: output/" + theConfiguration.getOutputName() + "/" + province.second->getHistoryCountryFile());
		output << *province.second;
		output.close();
	}
}

void EU4::World::outputLocalization(const Configuration& theConfiguration, bool invasion) const
{
	std::ofstream english("output/" + theConfiguration.getOutputName() + "/localisation/replace/converter_l_english.yml");
	std::ofstream french("output/" + theConfiguration.getOutputName() + "/localisation/replace/converter_l_french.yml");
	std::ofstream spanish("output/" + theConfiguration.getOutputName() + "/localisation/replace/converter_l_spanish.yml");
	std::ofstream german("output/" + theConfiguration.getOutputName() + "/localisation/replace/converter_l_german.yml");
	if (!english.is_open())
		throw std::runtime_error("Error writing localization file! Is the output folder writable?");
	if (!french.is_open())
		throw std::runtime_error("Error writing localization file! Is the output folder writable?");
	if (!spanish.is_open())
		throw std::runtime_error("Error writing localization file! Is the output folder writable?");
	if (!german.is_open())
		throw std::runtime_error("Error writing localization file! Is the output folder writable?");
	english << commonItems::utf8BOM << "l_english:\n"; // write BOM
	french << commonItems::utf8BOM << "l_french:\n";	// write BOM
	spanish << commonItems::utf8BOM << "l_spanish:\n"; // write BOM
	german << commonItems::utf8BOM << "l_german:\n";	// write BOM

	for (const auto& country: countries)
	{
		for (const auto& locblock: country.second->getLocalizations())
		{
			english << " " << locblock.first << ":3 \"" << locDegrader.degradeString(locblock.second.english) << "\"\n";
			french << " " << locblock.first << ":3 \"" << locDegrader.degradeString(locblock.second.french) << "\"\n";
			spanish << " " << locblock.first << ":3 \"" << locDegrader.degradeString(locblock.second.spanish) << "\"\n";
			german << " " << locblock.first << ":3 \"" << locDegrader.degradeString(locblock.second.german) << "\"\n";
		}
	}

	for (const auto& locblock: religionMapper.getLocalizations())
	{
		english << " " << locblock.first << ":3 \"" << locDegrader.degradeString(locblock.second.english) << "\"\n";
		french << " " << locblock.first << ":3 \"" << locDegrader.degradeString(locblock.second.french) << "\"\n";
		spanish << " " << locblock.first << ":3 \"" << locDegrader.degradeString(locblock.second.spanish) << "\"\n";
		german << " " << locblock.first << ":3 \"" << locDegrader.degradeString(locblock.second.german) << "\"\n";
	}

	english.close();
	french.close();
	spanish.close();
	german.close();

	if (invasion)
	{
		auto fileNames = commonItems::GetAllFilesInFolder("configurables/sunset/localisation/");
		for (const auto& fileName: fileNames)
			commonItems::TryCopyFile("configurables/sunset/localisation/" + fileName, "output/" + theConfiguration.getOutputName() + "/localisation/" + fileName);
	}

	auto fileNames = commonItems::GetAllFilesInFolder("configurables/religions/reformation/roman/");
	for (const auto& fileName: fileNames)
		commonItems::TryCopyFile("configurables/religions/reformation/roman/" + fileName,
			 "output/" + theConfiguration.getOutputName() + "/localisation/" + fileName);
}

void EU4::World::outputEmperor(const Configuration& theConfiguration, date conversionDate) const
{
	auto actualConversionDate = conversionDate;
	if (theConfiguration.getStartDate() == Configuration::STARTDATE::EU)
		actualConversionDate = date(1444, 11, 11);

	std::ofstream output("output/" + theConfiguration.getOutputName() + "/history/diplomacy/hre.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create hre diplomacy file: output/" + theConfiguration.getOutputName() + "/history/diplomacy/hre.txt!");
	if (emperorTag.empty())
		output << actualConversionDate << " = { emperor = --- }\n";
	else
		output << actualConversionDate << " = { emperor = " << emperorTag << " }\n";
	output.close();

	output.open("output/" + theConfiguration.getOutputName() + "/history/diplomacy/celestial_empire.txt");
	output << actualConversionDate << " = { celestial_emperor = MNG }\n"; // hardcoded until china dlc.
	output.close();

	if (!actualHRETag.empty())
	{
		output.open("output/" + theConfiguration.getOutputName() + "/i_am_hre.txt");
		output << actualHRETag;
		output.close();

		if (actualHRETag != "HRE")
		{
			std::ifstream input("output/" + theConfiguration.getOutputName() + "/events/HolyRomanEmpire.txt");
			std::stringstream inStream;
			inStream << input.rdbuf();
			auto eventFileString = inStream.str();
			input.close();
			eventFileString = std::regex_replace(eventFileString, std::regex("HLR"), actualHRETag);
			output.open("output/" + theConfiguration.getOutputName() + "/events/HolyRomanEmpire.txt");
			output << eventFileString;
			output.close();
		}
	}
}

void EU4::World::outputDiplomacy(const Configuration& theConfiguration, const std::vector<std::shared_ptr<Agreement>>& agreements, bool invasion) const
{
	std::ofstream alliances("output/" + theConfiguration.getOutputName() + "/history/diplomacy/converter_alliances.txt");
	if (!alliances.is_open())
		throw std::runtime_error("Could not create alliances history file!");

	std::ofstream guarantees("output/" + theConfiguration.getOutputName() + "/history/diplomacy/converter_guarantees.txt");
	if (!guarantees.is_open())
		throw std::runtime_error("Could not create guarantees history file!");

	std::ofstream puppetStates("output/" + theConfiguration.getOutputName() + "/history/diplomacy/converter_puppetstates.txt");
	if (!puppetStates.is_open())
		throw std::runtime_error("Could not create puppet states history file!");

	std::ofstream unions("output/" + theConfiguration.getOutputName() + "/history/diplomacy/converter_unions.txt");
	if (!unions.is_open())
		throw std::runtime_error("Could not create unions history file!");

	for (const auto& agreement: agreements)
	{
		if (agreement->getType() == "guarantee")
		{
			guarantees << *agreement;
		}
		else if (agreement->getType() == "union")
		{
			unions << *agreement;
		}
		else if (agreement->getType() == "vassal" || agreement->getType() == "dependency")
		{
			puppetStates << *agreement;
		}
		else if (agreement->getType() == "alliance")
		{
			alliances << *agreement;
		}
		else
		{
			Log(LogLevel::Warning) << "Cannot output diplomatic agreement type " << agreement->getType() << "!";
		}
	}

	alliances.close();
	guarantees.close();
	puppetStates.close();
	unions.close();

	if (invasion)
	{
		// Blank american diplomacy
		std::ofstream diplo("output/" + theConfiguration.getOutputName() + "/history/diplomacy/American_alliances.txt");
		diplo << "\n";
		diplo.close();
		// and move over our alliances.
		commonItems::TryCopyFile("configurables/sunset/history/diplomacy/SunsetInvasion.txt",
			 "output/" + theConfiguration.getOutputName() + "/history/diplomacy/SunsetInvasion.txt");
	}
}

void EU4::World::outputFlags(const Configuration& theConfiguration) const
{
	auto counter = 0;
	if (!commonItems::DoesFolderExist("flags.tmp"))
	{
		Log(LogLevel::Warning) << "Flag folder flags.tmp not found!";
		return;
	}
	for (const auto& filename: commonItems::GetAllFilesInFolder("flags.tmp"))
	{
		if (commonItems::TryCopyFile("flags.tmp/" + filename, "output/" + theConfiguration.getOutputName() + "/gfx/flags/" + filename))
			++counter;
	}
	Log(LogLevel::Info) << "<< " << counter << " flags copied.";
}
