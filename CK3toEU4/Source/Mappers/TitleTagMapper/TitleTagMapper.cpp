#include "TitleTagMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include <iomanip>

mappers::TitleTagMapper::TitleTagMapper()
{
	LOG(LogLevel::Info) << "-> Parsing Tag mappings";
	registerKeys();
	parseFile("configurables/tag_mappings.txt");
	clearRegisteredKeywords();
	LOG(LogLevel::Info) << "<> " << theMappings.size() << " mappings loaded.";
}

mappers::TitleTagMapper::TitleTagMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::TitleTagMapper::registerKeys()
{
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream) {
		theMappings.emplace_back(TitleTagMapping(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void mappers::TitleTagMapper::registerTitle(const std::string& ck3title, const std::string& eu4tag)
{
	registeredTitleTags.insert(std::pair(ck3title, eu4tag));
	usedTags.insert(eu4tag);
}

std::optional<std::string> mappers::TitleTagMapper::getTagForTitle(const std::string& ck3Title)
{
	return getTagForTitle(ck3Title, 0);
}

std::optional<std::string> mappers::TitleTagMapper::getTagForTitle(const std::string& ck3Title, int eu4Capital)
{
	// the only case where we fail is on invalid invocation. Otherwise, failure is
	// not an option!
	if (ck3Title.empty())
		return std::nullopt;

	// The popes don't use proper titles and aren't registered.
	if (ck3Title == "The Pope")
	{
		if (!usedTags.count("PAP"))
		{
			usedTags.insert("PAP");
			return "PAP";
		}
		else
		{
			auto generatedTag = generateNewTag();
			usedTags.insert(generatedTag);
			return generatedTag;
		}
	}

	// look up register
	const auto& registerItr = registeredTitleTags.find(ck3Title);
	if (registerItr != registeredTitleTags.end())
		return registerItr->second;

	// Map The Pope(s)
	if (ck3Title == "The Pope")
	{
		if (!usedTags.count("PAP"))
		{
			registerTitle(ck3Title, "PAP");
			return "PAP";
		}
	}

	// Attempt a capital match.
	if (eu4Capital)
	{
		for (const auto& mapping: theMappings)
		{
			const auto& match = mapping.capitalMatch(eu4Capital);
			if (match)
			{
				if (usedTags.count(*match))
					continue;
				registerTitle(ck3Title, *match);
				return *match;
			}
		}
	}

	// Attempt a title match
	for (const auto& mapping: theMappings)
	{
		const auto& match = mapping.titleMatch(ck3Title);
		if (match)
		{
			if (usedTags.count(*match))
				continue;
			registerTitle(ck3Title, *match);
			return *match;
		}
	}

	// Generate a new tag
	auto generatedTag = generateNewTag();
	registerTitle(ck3Title, generatedTag);
	return generatedTag;
}

std::string mappers::TitleTagMapper::generateNewTag()
{
	std::ostringstream generatedEU4TagStream;
	generatedEU4TagStream << generatedEU4TagPrefix << std::setfill('0') << std::setw(2) << generatedEU4TagSuffix;
	const auto& eu4Tag = generatedEU4TagStream.str();
	const std::set<char> reservedPrefixes = {'D', 'C', 'K', 'E', 'T', 'O'};

	++generatedEU4TagSuffix;
	if (generatedEU4TagSuffix > 99)
	{
		generatedEU4TagSuffix = 0;
		--generatedEU4TagPrefix;
		while (reservedPrefixes.count(generatedEU4TagPrefix))
			--generatedEU4TagPrefix;
	}

	return eu4Tag;
}
