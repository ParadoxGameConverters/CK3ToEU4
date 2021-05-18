#include "LocalizationMapper.h"
#include "../../Configuration/Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <fstream>
#include <set>

void mappers::LocalizationMapper::scrapeLocalizations(const Configuration& theConfiguration, const std::map<std::string, std::string>& mods)
{
	scrapeLanguage("english", theConfiguration.getCK3Path() + "localization");
	scrapeLanguage("french", theConfiguration.getCK3Path() + "localization");
	scrapeLanguage("german", theConfiguration.getCK3Path() + "localization");
	scrapeLanguage("spanish", theConfiguration.getCK3Path() + "localization");

	for (const auto& mod: mods)
	{
		if (commonItems::DoesFolderExist(mod.second + "localization"))
		{
			Log(LogLevel::Info) << "\t>> Found some words in: " << mod.second + "/localization";
			scrapeLanguage("english", mod.second + "/localization");
			scrapeLanguage("french", mod.second + "/localization");
			scrapeLanguage("german", mod.second + "/localization");
			scrapeLanguage("spanish", mod.second + "/localization");
			scrapeLanguage("english", mod.second + "/localization/replace");
			scrapeLanguage("french", mod.second + "/localization/replace");
			scrapeLanguage("german", mod.second + "/localization/replace");
			scrapeLanguage("spanish", mod.second + "/localization/replace");
		}
	}

	LOG(LogLevel::Info) << ">> " << localizations.size() << " words read.";
}

void mappers::LocalizationMapper::scrapeLanguage(const std::string& language, const std::string& path)
{
	if (!commonItems::DoesFolderExist(path + "/" + language))
		return;
	auto fileNames = commonItems::GetAllFilesInFolderRecursive(path + "/" + language);
	for (const auto& file: fileNames)
	{
		std::ifstream fileStream(path + "/" + language + "/" + file);
		if (fileStream.is_open())
			scrapeStream(fileStream, language);
		fileStream.close();
	}
}

void mappers::LocalizationMapper::scrapeStream(std::istream& theStream, const std::string& language)
{
	std::string line;
	std::getline(theStream, line); // Dropping first line and BOM with it.

	while (!theStream.eof())
	{
		std::getline(theStream, line);
		const auto [key, value] = determineKeyLocalizationPair(line);
		if (!key.empty() && !value.empty())
		{
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
}

std::pair<std::string, std::string> mappers::LocalizationMapper::determineKeyLocalizationPair(const std::string& text)
{
	std::pair<std::string, std::string> blankReturn;

	if (text.size() > 2 && (text[0] == '#' || text[1] == '#'))
		return blankReturn;

	const auto keyBeginPos = text.find_first_not_of(' ');
	if (keyBeginPos == std::string::npos)
		return blankReturn;

	const auto keyEndPos = text.find_first_of(':', keyBeginPos + 1);
	const auto quotePos = text.find_first_of('"', keyEndPos);
	if (quotePos == std::string::npos)
		return blankReturn;

	const auto localizationBeginPos = quotePos + 1;
	const auto localizationEndPos = text.find_last_of('"', text.size());
	return std::make_pair(text.substr(keyBeginPos, keyEndPos - keyBeginPos), text.substr(localizationBeginPos, localizationEndPos - localizationBeginPos));
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
		return std::move(newBlock);
	}
	// either all is well, or we're missing english. Can't do anything about the latter.
	return keyItr->second;
}