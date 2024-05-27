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

void mappers::LocalizationMapper::unravelNestedLocs(LocBlock& block) const
{
	// Support Korean/Chinese/Russian?
	for (const auto& lang: std::vector<std::string>{"english", "french", "spanish", "german"})
	{
		const auto& loc = selectLanguage(lang, block);
		if (loc.find('$') != std::string::npos) // TODO: handle escaped \$
		{
			const auto& keyStr = getLeadStr(loc, 2, "$");		 // Chop off tail after nested key
			const auto& nestedKey = getTailStr(keyStr, 1, "$"); // Chop off head before nested key
			if (const auto& newblock = getLocBlockForKey(nestedKey); newblock)
			{
				const auto& fstr = getLeadStr(loc, 1, "$");
				const auto& bstr = getTailStr(loc, 2, "$");
				assignSelectLanguage(fstr + selectLanguage(lang, newblock.value()) + bstr, lang, block);
			}
			else
			{
				Log(LogLevel::Warning) << "Could not find locblock for nested loc: " + nestedKey;
				return;
			}
			unravelNestedLocs(block);
		}
	}
	return;
}

void mappers::LocalizationMapper::removeEmphasis(LocBlock& block) const
{
	for (const auto& lang: std::vector<std::string>{"english", "french", "spanish", "german"})
	{
		const auto& loc = selectLanguage(lang, block);
		assignSelectLanguage(cleanLocMarkups(loc), lang, block);
	}
}

const std::string mappers::LocalizationMapper::selectLanguage(const std::string& language, const LocBlock& block) const
{
	if (language == "english")
		return block.english;
	else if (language == "french")
		return block.french;
	else if (language == "spanish")
		return block.spanish;
	else if (language == "german")
		return block.german;
	else if (language == "korean")
		return block.korean;
	else if (language == "russian")
		return block.russian;
	else if (language == "simp_chinese")
		return block.simp_chinese;
	else
		return block.english;
}

void mappers::LocalizationMapper::assignSelectLanguage(const std::string& str, const std::string& language, LocBlock& block) const
{
	if (language == "english")
		block.english = commonItems::remQuotes(str);
	else if (language == "french")
		block.french = commonItems::remQuotes(str);
	else if (language == "spanish")
		block.spanish = commonItems::remQuotes(str);
	else if (language == "german")
		block.german = commonItems::remQuotes(str);
	else if (language == "korean")
		block.korean = commonItems::remQuotes(str);
	else if (language == "russian")
		block.russian = commonItems::remQuotes(str);
	else if (language == "simp_chinese")
		block.simp_chinese = commonItems::remQuotes(str);
	else
		throw std::invalid_argument(language + " is not currently supported or has a typo.");
	return;
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
	if (key.empty())
		return std::nullopt;
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
		return newBlock;
	}

	// either all is well, or we're missing english. Can't do anything about the latter. Since English is absolute base, we bail if it's missing.
	// (Quite a few locs have only russian entries and no other languages.)
	if (keyItr->second.english.empty())
		return std::nullopt;

	return keyItr->second;
}

std::optional<std::string> mappers::LocalizationMapper::reverseLookupCultureName(const std::string& localization) const
{
	// This looks for specifically *_name keys cultures use.

	for (const auto& [locName, locBlock]: localizations)
	{
		if (locBlock.english == localization || locBlock.french == localization || locBlock.german == localization || locBlock.korean == localization ||
			 locBlock.russian == localization || locBlock.simp_chinese == localization || locBlock.spanish == localization)
			if (locName.find("_name") != std::string::npos)
				return locName;
	}
	return std::nullopt;
}

std::string mappers::getLeadStr(const std::string& str, const int occurrence, const std::string& match)
{
	if (const auto& i = str.find(match); i != std::string::npos)
		if (occurrence == 1)
			return str.substr(0, i);
		else
			return str.substr(0, i) + match + getLeadStr(str.substr(i + match.length()), occurrence - 1, match);
	else
		return str;
}
std::string mappers::getTailStr(const std::string& str, const int occurrence, const std::string& match)
{
	if (const auto& i = str.find(match); i != std::string::npos)
		if (occurrence == 1)
		{
			auto tail = str.substr(i + match.length());
			if (tail.starts_with("!"))
				tail = tail.substr(1, tail.length());
			return tail;
		}
		else
			return getTailStr(str.substr(i + match.length()), occurrence - 1, match);
	else
		return str;
}
std::string mappers::cleanLocMarkups(const std::string& loc)
{
	if (loc.find('#') == std::string::npos)
		return loc;

	// Locmarks come in two styles: #SOMETHING with a whitespace behind it, and a #! witho no whitespace trailing it.
	// We iterate over the entire string and just rip these out.

	auto workingLoc = loc;
	auto pos = workingLoc.find('#');

	while (pos != std::string::npos)
	{
		// find first #
		const auto& head = getLeadStr(workingLoc, 1, "#");

		// find how long it is.
		if (pos <= workingLoc.size() - 2 && workingLoc.at(pos + 1) == '!')
		{
			// Delete #!
			workingLoc = head + workingLoc.substr(pos + 2, workingLoc.size());
		}
		else if (pos <= workingLoc.size() - 2 && workingLoc.at(pos + 1) == ' ')
		{
			// Delete freeform #
			workingLoc = head + workingLoc.substr(pos + 1, workingLoc.size());
		}
		else if (pos == workingLoc.size() - 1)
		{
			// Delete # at endline
			workingLoc = head;
		}
		else
		{
			// find the whitespace ending the mark.
			auto endpos = workingLoc.find(' ', pos + 1);
			if (endpos == std::string::npos)
				return loc; // bail
			workingLoc = head + workingLoc.substr(endpos + 1, workingLoc.size());
		}
		pos = workingLoc.find('#');
	}

	return workingLoc;
}