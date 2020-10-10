#include "Mods.h"
#include "../../Configuration/Configuration.h"
#include "CommonFunctions.h"
#include "Log.h"
#include "Mod.h"
#include "OSCompatibilityLayer.h"
#include "ZipFile.h"
#include <filesystem>
#include <fstream>
#include <set>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

void CK3::Mods::loadModDirectory(const Configuration& theConfiguration)
{
	if (theConfiguration.getModFileNames().empty())
	{
		Log(LogLevel::Info) << "No mods were detected in savegame. Skipping mod processing.";
		return;
	}

	loadCK3ModDirectory(theConfiguration);

	Log(LogLevel::Info) << "\tDetermining Mod Usability";
	auto allMods = possibleMods;
	allMods.insert(possibleCompressedMods.begin(), possibleCompressedMods.end());
	for (const auto& usedMod: allMods)
	{
		auto possibleModPath = getModPath(usedMod.first);
		if (possibleModPath)
		{
			if (!commonItems::DoesFolderExist(*possibleModPath) && !commonItems::DoesFileExist(*possibleModPath))
			{
				Log(LogLevel::Warning) << usedMod.first + " could not be found in the specified mod directory " +
														"- a valid mod directory must be specified. Tried " + *possibleModPath;
				continue;
			}

			LOG(LogLevel::Info) << "\t\t->> Found potentially usable [" << usedMod.first << "]: " << *possibleModPath + "/";
			usableMods.insert(std::pair(usedMod.first, *possibleModPath + "/"));
		}
		else
		{
			Log(LogLevel::Warning) << "No path could be found for " + usedMod.first +
													". Check that the mod is present and that the .mod file specifies the path for the mod";
		}
	}
}

void CK3::Mods::loadCK3ModDirectory(const Configuration& theConfiguration)
{
	const auto& CK3ModsPath = theConfiguration.getCK3DocPath() + "/mod";
	if (!commonItems::DoesFolderExist(CK3ModsPath))
		throw std::invalid_argument("Crusader Kings 3 mods directory path is invalid! Is it at: " + theConfiguration.getCK3DocPath() + "/mod/ ?");

	LOG(LogLevel::Info) << "\tCK3 mods directory is " << CK3ModsPath;

	auto filenames = commonItems::GetAllFilesInFolder(CK3ModsPath);
	for (const auto& filename: filenames)
	{
		if (!theConfiguration.getModFileNames().count("mod/" + filename))
			continue; // Mod was not enabled by configuration, so move on.
		if (fs::path(filename).extension() != ".mod")
			continue; // shouldn't be necessary but just in case.
		try
		{
			std::ifstream modFile(fs::u8path(CK3ModsPath + "/" + filename));
			Mod theMod(modFile);
			modFile.close();

			if (theMod.looksValid())
			{
				if (!theMod.isCompressed())
				{
					if (!commonItems::DoesFolderExist(theMod.getPath()))
					{
						// Maybe we have a relative path
						if (commonItems::DoesFolderExist(theConfiguration.getCK3DocPath() + "/" + theMod.getPath()))
						{
							// fix this.
							theMod.setPath(theConfiguration.getCK3DocPath() + "/" + theMod.getPath());
						}
						else
						{
							throw std::invalid_argument("Mod file " + filename + " points to " + theMod.getPath() + " which does not exist!");
						}
					}

					possibleMods.insert(std::make_pair(theMod.getName(), theMod.getPath()));
					Log(LogLevel::Info) << "\t\tFound potential mod named " << theMod.getName() << " with a mod file at " << CK3ModsPath + "/" + filename
											  << " and itself at " << theMod.getPath();
				}
				else
				{
					if (!commonItems::DoesFileExist(theMod.getPath()))
					{
						throw std::invalid_argument("Mod file " + filename + " points to " + theMod.getPath() + " which does not exist!");
					}

					possibleCompressedMods.insert(std::make_pair(theMod.getName(), theMod.getPath()));
					Log(LogLevel::Info) << "\t\tFound a compressed mod named " << theMod.getName() << " with a mod file at " << CK3ModsPath << "/" << filename
											  << " and itself at " << theMod.getPath();
				}
			}
			else
			{
				Log(LogLevel::Warning) << "Mod at " << CK3ModsPath + "/" + filename << " does not look valid.";
			}
		}
		catch (std::exception&)
		{
			LOG(LogLevel::Warning) << "Error while reading " << CK3ModsPath << "/" << filename << "! Mod will not be useable for conversions.";
		}
	}
}

std::optional<std::string> CK3::Mods::getModPath(const std::string& modName) const
{
	const auto& mod = possibleMods.find(modName);
	if (mod != possibleMods.end())
	{
		return mod->second;
	}

	const auto& compressedMod = possibleCompressedMods.find(modName);
	if (compressedMod != possibleCompressedMods.end())
	{
		const auto archivePath = compressedMod->second;
		const auto uncompressedName = trimPath(trimExtension(archivePath));

		commonItems::TryCreateFolder("mods/");

		if (!commonItems::DoesFolderExist("mods/" + uncompressedName))
		{
			LOG(LogLevel::Info) << "\t\tUncompressing: " << archivePath;
			if (!extractZip(archivePath, "mods/" + uncompressedName))
			{
				LOG(LogLevel::Warning) << "We're having trouble automatically uncompressing your mod.";
				LOG(LogLevel::Warning) << "Please, manually uncompress: " << archivePath;
				LOG(LogLevel::Warning) << "Into: CK3toEU4/mods/" << uncompressedName;
				LOG(LogLevel::Warning) << "Then run the converter again. Thank you and good luck.";
				return std::nullopt;
			}
		}

		if (commonItems::DoesFolderExist("mods/" + uncompressedName))
		{
			return "mods/" + uncompressedName;
		}
	}

	return std::nullopt;
}

bool CK3::Mods::extractZip(const std::string& archive, const std::string& path) const
{
	commonItems::TryCreateFolder(path);
	auto modfile = ZipFile::Open(archive);
	if (!modfile)
		return false;
	for (size_t entryNum = 0; entryNum < modfile->GetEntriesCount(); ++entryNum)
	{
		const auto& entry = modfile->GetEntry(static_cast<int>(entryNum));
		const auto& inpath = entry->GetFullName();
		const auto& name = entry->GetName();
		if (entry->IsDirectory())
			continue;

		// Does target directory exist?
		const auto dirnamepos = inpath.find(name);
		const auto dirname = path + "/" + inpath.substr(0, dirnamepos);
		if (!commonItems::DoesFolderExist(dirname))
		{
			// we need to craft our way through to target directory.
			auto remainder = inpath;
			auto currentpath = path;
			while (remainder != name)
			{
				const auto pos = remainder.find_first_of('/');
				if (pos != std::string::npos)
				{
					auto makedirname = remainder.substr(0, pos);
					currentpath += "/" + makedirname;
					commonItems::TryCreateFolder(currentpath);
					remainder = remainder.substr(pos + 1, remainder.length());
				}
				else
					break;
			}
		}
		ZipFile::ExtractFile(archive, inpath, path + "/" + inpath);
	}
	return true;
}