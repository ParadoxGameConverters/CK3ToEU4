#include "../EU4World.h"
#include "../../CK3World/World.h"
#include "Log.h"
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;
#include "../../Configuration/Configuration.h"
#include "OSCompatibilityLayer.h"

void EU4::World::output(const mappers::ConverterVersion& converterVersion, const Configuration& theConfiguration, const CK3::World& sourceWorld) const
{
	const auto invasion = theConfiguration.getSunset() == Configuration::SUNSET::ACTIVE;
	const date conversionDate = sourceWorld.getConversionDate();
	LOG(LogLevel::Info) << "<- Creating Output Folder";

	Utils::TryCreateFolder("output");
	if (Utils::DoesFolderExist("output/" + theConfiguration.getOutputName()))
	{
		Log(LogLevel::Info) << "<< Deleting existing mod folder.";
		Utils::DeleteFolder("output/" + theConfiguration.getOutputName());
	}
	Log(LogLevel::Progress) << "80 %";

	LOG(LogLevel::Info) << "<- Copying Mod Template";
	Utils::CopyFolder("blankMod/output", "output/output");
	Log(LogLevel::Progress) << "81 %";

	LOG(LogLevel::Info) << "<- Moving Mod Template >> " << theConfiguration.getOutputName();
	Utils::RenameFolder("output/output", "output/" + theConfiguration.getOutputName());
	Log(LogLevel::Progress) << "82 %";

	Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/");
	Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/countries/");
	Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/advisors/");
	Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/provinces/");
	Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/history/diplomacy/");
	Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/common/");
	Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/common/countries/");
	Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/common/country_tags/");
	Utils::TryCreateFolder("output/" + theConfiguration.getOutputName() + "/localisation/");
	Log(LogLevel::Progress) << "83 %";

	LOG(LogLevel::Info) << "<- Crafting .mod File";
	createModFile(theConfiguration);
	Log(LogLevel::Progress) << "84 %";

	// Record converter version
	LOG(LogLevel::Info) << "<- Writing version";
	outputVersion(converterVersion, theConfiguration);
	Log(LogLevel::Progress) << "85 %";

	if (invasion)
	{
		LOG(LogLevel::Info) << "<- Writing Sunset Invasion Files";
		outputInvasionExtras(theConfiguration);
	}

	LOG(LogLevel::Info) << "<- Replacing Bookmark";
	outputBookmark(theConfiguration, conversionDate);
	Log(LogLevel::Progress) << "95 %";
}

void EU4::World::outputBookmark(const Configuration& theConfiguration, date conversionDate) const
{
	if (!Utils::DoesFileExist("output/" + theConfiguration.getOutputName() + "/common/defines/00_converter_defines.lua"))
		throw std::runtime_error("Can not find output/" + theConfiguration.getOutputName() + "/common/defines/00_converter_defines.lua!");
	std::ofstream defines("output/" + theConfiguration.getOutputName() + "/common/defines/00_converter_defines.lua");
	defines << "-- Defines modified by the converter\n\n";
	defines << "\nNDefines.NGame.START_DATE = \"" << conversionDate << "\"\n";
	defines.close();
	if (!Utils::DoesFileExist("output/" + theConfiguration.getOutputName() + "/common/bookmarks/converter_bookmark.txt"))
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
}

void EU4::World::createModFile(const Configuration& theConfiguration) const
{
	std::ofstream output("output/" + theConfiguration.getOutputName() + ".mod");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + theConfiguration.getOutputName() + ".mod");
	LOG(LogLevel::Info) << "<< Writing to: "
							  << "output/" + theConfiguration.getOutputName() + ".mod";
	output << modFile;
	output.close();

	std::ofstream output2("output/" + theConfiguration.getOutputName() + "/descriptor.mod");
	if (!output2.is_open())
		throw std::runtime_error("Could not create " + theConfiguration.getOutputName() + "/descriptor.mod");
	LOG(LogLevel::Info) << "<< Writing to: "
							  << "output/" + theConfiguration.getOutputName() + "/descriptor.mod";
	output2 << modFile;
	output2.close();
}

void EU4::World::outputVersion(const mappers::ConverterVersion& converterVersion, const Configuration& theConfiguration) const
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
	auto files = Utils::GetAllFilesInFolder("configurables/sunset/common/religions/");
	for (const auto& file: files)
		Utils::TryCopyFile("configurables/sunset/common/religions/" + file, "output/" + theConfiguration.getOutputName() + "/common/religions/" + file);
	// Sunset Ideas
	files = Utils::GetAllFilesInFolder("configurables/sunset/common/ideas/");
	for (const auto& file: files)
		Utils::TryCopyFile("configurables/sunset/common/ideas/" + file, "output/" + theConfiguration.getOutputName() + "/common/ideas/" + file);
	// Sunset Cultures
	files = Utils::GetAllFilesInFolder("configurables/sunset/common/cultures/");
	for (const auto& file: files)
		Utils::TryCopyFile("configurables/sunset/common/cultures/" + file, "output/" + theConfiguration.getOutputName() + "/common/cultures/" + file);
	// Sunset Decisions
	files = Utils::GetAllFilesInFolder("configurables/sunset/decisions/");
	for (const auto& file: files)
		Utils::TryCopyFile("configurables/sunset/decisions/" + file, "output/" + theConfiguration.getOutputName() + "/decisions/" + file);
}
