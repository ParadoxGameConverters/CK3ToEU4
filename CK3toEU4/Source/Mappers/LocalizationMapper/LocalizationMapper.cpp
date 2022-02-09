#include "LocalizationMapper.h"
#include "../../Configuration/Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <fstream>
#include <set>

void mappers::LocalizationMapper::scrapeLocalizations(const Configuration& theConfiguration, const Mods& mods)
{
	scrapeLanguage("english", theConfiguration.getCK3Path() + "localization");
	scrapeLanguage("french", theConfiguration.getCK3Path() + "localization");
	scrapeLanguage("german", theConfiguration.getCK3Path() + "localization");
	scrapeLanguage("korean", theConfiguration.getCK3Path() + "/localization");
	scrapeLanguage("russian", theConfiguration.getCK3Path() + "/localization");
	scrapeLanguage("simp_chinese", theConfiguration.getCK3Path() + "/localization");
	scrapeLanguage("spanish", theConfiguration.getCK3Path() + "localization");

	for (const auto& mod: mods)
	{
		if (commonItems::DoesFolderExist(mod.path + "localization"))
		{
			Log(LogLevel::Info) << "\t>> Found some words in: [" << mod.name + "]";
			scrapeLanguage("english", mod.path + "/localization");
			scrapeLanguage("french", mod.path + "/localization");
			scrapeLanguage("german", mod.path + "/localization");
			scrapeLanguage("korean", mod.path + "/localization");
			scrapeLanguage("russian", mod.path + "/localization");
			scrapeLanguage("simp_chinese", mod.path + "/localization");
			scrapeLanguage("spanish", mod.path + "/localization");
			scrapeLanguage("english", mod.path + "/localization/replace");
			scrapeLanguage("french", mod.path + "/localization/replace");
			scrapeLanguage("german", mod.path + "/localization/replace");
			scrapeLanguage("korean", mod.path + "/localization/replace");
			scrapeLanguage("russian", mod.path + "/localization/replace");
			scrapeLanguage("simp_chinese", mod.path + "/localization/replace");
			scrapeLanguage("spanish", mod.path + "/localization/replace");
		}
	}

	Log(LogLevel::Info) << ">> " << localizations.size() << " words read.";
}

void mappers::LocalizationMapper::scrapeLanguage(const std::string& language, const std::string& path)
{
	if (!commonItems::DoesFolderExist(path + "/" + language))
		return;
	const auto fileNames = commonItems::GetAllFilesInFolderRecursive(path + "/" + language);
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
			if (localizations.contains(key))
			{
				if (language == "english")
					localizations[key].english = value;
				if (language == "french")
					localizations[key].french = value;
				if (language == "german")
					localizations[key].german = value;
				if (language == "korean")
					localizations[key].korean = value;
				if (language == "russian")
					localizations[key].russian = value;
				if (language == "simp_chinese")
					localizations[key].simp_chinese = value;
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
				if (language == "korean")
					newBlock.korean = value;
				if (language == "russian")
					newBlock.russian = value;
				if (language == "simp_chinese")
					newBlock.simp_chinese = value;
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

	if (!keyItr->second.english.empty() && (keyItr->second.spanish.empty() || keyItr->second.german.empty() || keyItr->second.french.empty() ||
															 keyItr->second.korean.empty() || keyItr->second.russian.empty() || keyItr->second.simp_chinese.empty()))
	{
		auto newBlock = keyItr->second;
		if (newBlock.spanish.empty())
			newBlock.spanish = newBlock.english;
		if (newBlock.german.empty())
			newBlock.german = newBlock.english;
		if (newBlock.french.empty())
			newBlock.french = newBlock.english;
		if (newBlock.korean.empty())
			newBlock.korean = newBlock.english;
		if (newBlock.russian.empty())
			newBlock.russian = newBlock.english;
		if (newBlock.simp_chinese.empty())
			newBlock.simp_chinese = newBlock.english;
		return std::move(newBlock);
	}
	// either all is well, or we're missing english. Can't do anything about the latter.
	return keyItr->second;
}

std::optional<std::string> mappers::LocalizationMapper::reverseLookup(const std::string& localization) const
{
	for (const auto& [locName, locBlock]: localizations)
	{
		if (locBlock.english == localization || locBlock.french == localization || locBlock.german == localization || locBlock.korean == localization ||
			 locBlock.russian == localization || locBlock.simp_chinese == localization || locBlock.spanish == localization)
			return locName;
	}
	return std::nullopt;
}
