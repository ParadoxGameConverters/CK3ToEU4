#include "Configuration.h"
#include "Color.h"
#include "CommonFunctions.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
auto laFabricaDeColor = commonItems::Color::Factory();
#include "CommonRegexes.h"

Configuration::Configuration()
{
	LOG(LogLevel::Info) << "Reading configuration file";
	registerKeys();
	parseFile("configuration.txt");
	clearRegisteredKeywords();
	setOutputName();
	verifyCK3Path();
	verifyEU4Path();
	Log(LogLevel::Progress) << "3 %";
}

Configuration::Configuration(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
	setOutputName();
}

void Configuration::registerKeys()
{
	registerKeyword("SaveGame", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString path(theStream);
		SaveGamePath = path.getString();
		Log(LogLevel::Info) << "Save Game set to: " << SaveGamePath;
	});
	registerKeyword("CK3directory", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString path(theStream);
		CK3Path = path.getString();
	});
	registerKeyword("EU4directory", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString path(theStream);
		EU4Path = path.getString();
	});
	registerKeyword("CK3DocDirectory", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString path(theStream);
		CK3DocPath = path.getString();
	});
	registerKeyword("output_name", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString nameStr(theStream);
		outputName = nameStr.getString();
		Log(LogLevel::Info) << "Output name set to: " << outputName;
	});
	registerKeyword("start_date", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString startDateString(theStream);
		startDate = STARTDATE(std::stoi(startDateString.getString()));
		Log(LogLevel::Info) << "Start date set to: " << startDateString.getString();
	});
	registerKeyword("i_am_hre", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString hreString(theStream);
		iAmHre = I_AM_HRE(std::stoi(hreString.getString()));
		Log(LogLevel::Info) << "HRE set to: " << hreString.getString();
	});
	registerKeyword("dejure", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString dejureString(theStream);
		dejure = DEJURE(std::stoi(dejureString.getString()));
		Log(LogLevel::Info) << "DeJure set to: " << dejureString.getString();
	});
	registerKeyword("split_vassals", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString splitVassalsString(theStream);
		splitVassals = SPLITVASSALS(std::stoi(splitVassalsString.getString()));
		Log(LogLevel::Info) << "Split Vassals set to: " << splitVassalsString.getString();
	});
	registerKeyword("shatter_empires", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString shatterEmpiresString(theStream);
		shatterEmpires = SHATTER_EMPIRES(std::stoi(shatterEmpiresString.getString()));
		Log(LogLevel::Info) << "Shatter Empires set to: " << shatterEmpiresString.getString();
	});
	registerKeyword("shatter_level", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString shatterLevelString(theStream);
		shatterLevel = SHATTER_LEVEL(std::stoi(shatterLevelString.getString()));
		Log(LogLevel::Info) << "Shatter Level set to: " << shatterLevelString.getString();
	});
	registerKeyword("shatter_hre_level", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString shatterHRELevelString(theStream);
		shatterHRELevel = SHATTER_HRE_LEVEL(std::stoi(shatterHRELevelString.getString()));
		Log(LogLevel::Info) << "Shatter HRE Level set to: " << shatterHRELevelString.getString();
	});
	registerKeyword("siberia", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString siberiaString(theStream);
		siberia = SIBERIA(std::stoi(siberiaString.getString()));
		Log(LogLevel::Info) << "Siberia set to: " << siberiaString.getString();
	});
	registerKeyword("sunset", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString sunsetString(theStream);
		sunset = SUNSET(std::stoi(sunsetString.getString()));
		Log(LogLevel::Info) << "Sunset set to: " << sunsetString.getString();
	});
	registerKeyword("development", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString developmentString(theStream);
		development = DEVELOPMENT(std::stoi(developmentString.getString()));
		Log(LogLevel::Info) << "Development set to: " << developmentString.getString();
	});
	registerKeyword("selectedMods", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::stringList modsList(theStream);
		const auto& theList = modsList.getStrings();
		modFileNames.insert(theList.begin(), theList.end());
		Log(LogLevel::Info) << modFileNames.size() << " mods selected by configuration. Deselected mods will be ignored.";
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void Configuration::verifyCK3Path()
{
	if (!commonItems::DoesFolderExist(CK3Path))
		throw std::runtime_error(CK3Path + " does not exist!");
	// TODO: OSX and Linux paths are speculative
	if (!commonItems::DoesFileExist(CK3Path + "/binaries/ck3.exe") && !commonItems::DoesFileExist(CK3Path + "/CK3game") && !commonItems::DoesFileExist(CK3Path + "/binaries/ck3"))
		throw std::runtime_error(CK3Path + " does not contain Crusader Kings 3!");
	if (!commonItems::DoesFileExist(CK3Path + "/game/map_data/positions.txt"))
		throw std::runtime_error(CK3Path + " does not appear to be a valid CK3 install!");
	LOG(LogLevel::Info) << "\tCK3 install path is " << CK3Path;
	CK3Path += "/game/"; // We're adding "/game/" since all we ever need from now on is in that subdirectory.
}

void Configuration::verifyEU4Path() const
{
	if (!commonItems::DoesFolderExist(EU4Path))
		throw std::runtime_error(EU4Path + " does not exist!");
	if (!commonItems::DoesFileExist(EU4Path + "/eu4.exe") && !commonItems::DoesFileExist(EU4Path + "/eu4"))
		throw std::runtime_error(EU4Path + " does not contain Europa Universalis 4!");
	if (!commonItems::DoesFileExist(EU4Path + "/map/positions.txt"))
		throw std::runtime_error(EU4Path + " does not appear to be a valid EU4 install!");
	LOG(LogLevel::Info) << "\tEU4 install path is " << EU4Path;
}

void Configuration::setOutputName()
{
	if (outputName.empty())
	{
		outputName = trimPath(SaveGamePath);
	}
	outputName = trimExtension(outputName);
	outputName = replaceCharacter(outputName, '-');
	outputName = replaceCharacter(outputName, ' ');

	outputName = commonItems::normalizeUTF8Path(outputName);
	LOG(LogLevel::Info) << "Using output name " << outputName;
}
