#include "Configuration.h"
#include "Color.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "GameVersion.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <fstream>
auto laFabricaDeColor = commonItems::Color::Factory();

Configuration::Configuration(const commonItems::ConverterVersion& converterVersion)
{
	Log(LogLevel::Info) << "Reading configuration file";
	registerKeys();
	parseFile(std::filesystem::path("configuration.txt"));
	clearRegisteredKeywords();
	setOutputName();
	verifyCK3Path();
	verifyCK3Version(converterVersion);
	verifyEU4Path();
	verifyEU4Version(converterVersion);
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
	registerKeyword("dynamicInstitutions", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString dynamicInstitutionsString(theStream);
		dynamicInstitutions = INSTITUTIONS(std::stoi(dynamicInstitutionsString.getString()));
		Log(LogLevel::Info) << "Dynamic Institutions set to: " << dynamicInstitutionsString.getString();
	});
	registerKeyword("development", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString developmentString(theStream);
		development = DEVELOPMENT(std::stoi(developmentString.getString()));
		Log(LogLevel::Info) << "Development set to: " << developmentString.getString();
	});
	registerKeyword("multiprovdevtransfer", [this](std::istream& theStream) {
		const commonItems::singleString multiprovdevtransferString(theStream);
		multiProvinceDevelopmentTransfer = MULTIPROVDEVTRANSFER(std::stoi(multiprovdevtransferString.getString()));
		Log(LogLevel::Info) << "Multiple Province Dev Transfer set to: " << multiprovdevtransferString.getString();
	});
	registerKeyword("dynasticNames", [this](std::istream& theStream) {
		const commonItems::singleString dynasticNamesString(theStream);
		dynasticNames = DYNASTICNAMES(std::stoi(dynasticNamesString.getString()));
		Log(LogLevel::Info) << "Dynastic Names set to: " << dynasticNamesString.getString();
	});
	registerKeyword("discoveredBy", [this](std::istream& theStream) {
		const commonItems::singleString discoveredByString(theStream);
		discoveredBy = DISCOVEREDBY(std::stoi(discoveredByString.getString()));
		Log(LogLevel::Info) << "Discovered By set to: " << discoveredByString.getString();
	});
	registerKeyword("tribalConfederations", [this](std::istream& theStream) {
		const commonItems::singleString tribalConfederationsString(theStream);
		tribalConfederations = TRIBALCONFEDERATIONS(std::stoi(tribalConfederationsString.getString()));
		Log(LogLevel::Info) << "Tribal Confederations set to: " << tribalConfederationsString.getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void Configuration::verifyCK3Path()
{
	if (!commonItems::DoesFolderExist(CK3Path))
		throw std::runtime_error(CK3Path.string() + " does not exist!");
	// TODO: OSX and Linux paths are speculative
	if (!commonItems::DoesFileExist(CK3Path / "binaries/ck3.exe") && !commonItems::DoesFileExist(CK3Path / "CK3game") &&
		 !commonItems::DoesFileExist(CK3Path / "binaries/ck3"))
		throw std::runtime_error(CK3Path.string() + " does not contain Crusader Kings 3!");
	if (!commonItems::DoesFileExist(CK3Path / "game/map_data/positions.txt"))
		throw std::runtime_error(CK3Path.string() + " does not appear to be a valid CK3 install!");
	Log(LogLevel::Info) << "\tCK3 install path is " << CK3Path;
	CK3Path += "/game/"; // We're adding "/game/" since all we ever need from now on is in that subdirectory.
}

void Configuration::verifyEU4Path() const
{
	if (!commonItems::DoesFolderExist(EU4Path))
		throw std::runtime_error(EU4Path.string() + " does not exist!");
	if (!commonItems::DoesFileExist(EU4Path / "eu4.exe") && !commonItems::DoesFileExist(EU4Path / "eu4"))
		throw std::runtime_error(EU4Path.string() + " does not contain Europa Universalis 4!");
	if (!commonItems::DoesFileExist(EU4Path / "map/positions.txt"))
		throw std::runtime_error(EU4Path.string() + " does not appear to be a valid EU4 install!");
	Log(LogLevel::Info) << "\tEU4 install path is " << EU4Path;
}

void Configuration::setOutputName()
{
	if (outputName.empty())
	{
		outputName = SaveGamePath.filename();
	}
	auto outputNameString = outputName.stem().string();
	outputNameString = replaceCharacter(outputNameString, '-');
	outputNameString = replaceCharacter(outputNameString, ' ');

	outputName = commonItems::normalizeUTF8Path(outputNameString);
	Log(LogLevel::Info) << "Using output name " << outputName.string();
}

void Configuration::verifyCK3Version(const commonItems::ConverterVersion& converterVersion) const
{
	const auto CK3Version = GameVersion::extractVersionFromLauncher(CK3Path / "../launcher/launcher-settings.json");
	if (!CK3Version)
	{
		Log(LogLevel::Error) << "CK3 version could not be determined, proceeding blind!";
		return;
	}

	Log(LogLevel::Info) << "CK3 version: " << CK3Version->toShortString();

	if (converterVersion.getMinSource() > *CK3Version)
	{
		Log(LogLevel::Error) << "CK3 version is v" << CK3Version->toShortString() << ", converter requires minimum v"
									<< converterVersion.getMinSource().toShortString() << "!";
		throw std::runtime_error("Converter vs CK3 installation mismatch!");
	}
	if (!converterVersion.getMaxSource().isLargerishThan(*CK3Version))
	{
		Log(LogLevel::Error) << "CK3 version is v" << CK3Version->toShortString() << ", converter requires maximum v"
									<< converterVersion.getMaxSource().toShortString() << "!";
		throw std::runtime_error("Converter vs CK3 installation mismatch!");
	}
}

void Configuration::verifyEU4Version(const commonItems::ConverterVersion& converterVersion) const
{
	const auto EU4Version = GameVersion::extractVersionFromLauncher(EU4Path / "launcher-settings.json");
	if (!EU4Version)
	{
		Log(LogLevel::Error) << "EU4 version could not be determined, proceeding blind!";
		return;
	}

	Log(LogLevel::Info) << "EU4 version: " << EU4Version->toShortString();

	if (converterVersion.getMinTarget() > *EU4Version)
	{
		Log(LogLevel::Error) << "EU4 version is v" << EU4Version->toShortString() << ", converter requires minimum v"
									<< converterVersion.getMinTarget().toShortString() << "!";
		throw std::runtime_error("Converter vs EU4 installation mismatch!");
	}
	if (!converterVersion.getMaxTarget().isLargerishThan(*EU4Version))
	{
		Log(LogLevel::Error) << "EU4 version is v" << EU4Version->toShortString() << ", converter requires maximum v"
									<< converterVersion.getMaxTarget().toShortString() << "!";
		throw std::runtime_error("Converter vs EU4 installation mismatch!");
	}
}
