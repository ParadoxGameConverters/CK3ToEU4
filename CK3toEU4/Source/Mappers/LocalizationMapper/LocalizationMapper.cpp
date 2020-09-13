#include "LocalizationMapper.h"
#include "../../Configuration/Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <fstream>
#include <set>

void mappers::LocalizationMapper::scrapeLocalizations(const Configuration& theConfiguration, const std::map<std::string, std::string>& mods)
{
	LOG(LogLevel::Info) << "-> Reading Words";
	
	scrapeLanguage("english", theConfiguration.getCK3Path() + "localization");
	scrapeLanguage("french", theConfiguration.getCK3Path() + "localization");
	scrapeLanguage("german", theConfiguration.getCK3Path() + "localization");
	scrapeLanguage("spanish", theConfiguration.getCK3Path() + "localization");

	for (const auto& mod: mods)
	{
		if (Utils::DoesFolderExist(mod.second + "localization"))
		{
			Log(LogLevel::Info) << "\t>> Found some words in: " << mod.second + "localization";
			scrapeLanguage("english", mod.second);
			scrapeLanguage("french", mod.second);
			scrapeLanguage("german", mod.second);
			scrapeLanguage("spanish", mod.second);
		}
	}
	
	LOG(LogLevel::Info) << ">> " << localizations.size() << " words read.";
}

void mappers::LocalizationMapper::scrapeLanguage(const std::string& language, const std::string& path)
{
	if (!Utils::DoesFolderExist(path + "/" + language))
		return;
	auto filenames = Utils::GetAllFilesInFolderRecursive(path + "/" + language);
	for (const auto& file: filenames)
	{
		std::ifstream filestream(path + "/" + language + "/" + file);
		if (filestream.is_open())
			scrapeStream(filestream, language);
		filestream.close();
	}
}

void mappers::LocalizationMapper::scrapeStream(std::istream& theStream, const std::string& language)
{
	while (!theStream.eof())
	{
		std::string line;
		getline(theStream, line);

		if (line[0] == '#' || line.length() < 4)
			continue;

		const auto sepLoc = line.find_first_of(':');
		if (sepLoc == std::string::npos)
			continue;
		const auto key = line.substr(1, sepLoc);
		const auto newLine = line.substr(sepLoc + 1, line.length());
		const auto quoteLoc = newLine.find_first_of('\"');
		const auto quote2Loc = newLine.find_last_of('\"');
		if (quoteLoc == std::string::npos || quote2Loc == std::string::npos || quote2Loc - quoteLoc == 0)
			continue;
		const auto value = newLine.substr(quoteLoc, quote2Loc - quoteLoc);

		if (localizations.count(key))
		{
			if (language == "english")
				localizations[key].english = value;
			if (language == "french")
				localizations[key].french = value;
			if (language == "german")
				localizations[key].german = value;
			if (language == "spanish")
				localizations[key].spanish = value;			
		}
		else
		{
			LocBlock newBlock;
			if (language == "english")
				newBlock.english = value;
			if (language == "french")
				newBlock.french = value;
			if (language == "german")
				newBlock.german = value;
			if (language == "spanish")
				newBlock.spanish = value;			
			localizations.insert(std::pair(key, newBlock));
		}
	}
}

std::optional<mappers::LocBlock> mappers::LocalizationMapper::getLocBlockForKey(const std::string& key) const
{
	const auto& keyItr = localizations.find(key);
	if (keyItr == localizations.end())
		return std::nullopt;

	if (!keyItr->second.english.empty() && (keyItr->second.spanish.empty() || keyItr->second.german.empty() || keyItr->second.french.empty()))
	{
		auto newBlock = keyItr->second;
		if (newBlock.spanish.empty())
			newBlock.spanish = newBlock.english;
		if (newBlock.german.empty())
			newBlock.german = newBlock.english;
		if (newBlock.french.empty())
			newBlock.french = newBlock.english;
		return newBlock;
	}
	// either all is well, or we're missing english. Can't do anything about the latter.
	return keyItr->second;
}