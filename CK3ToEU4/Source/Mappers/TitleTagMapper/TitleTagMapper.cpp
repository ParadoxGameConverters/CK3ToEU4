#include "TitleTagMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include <iomanip>

mappers::TitleTagMapper::TitleTagMapper()
{
	Log(LogLevel::Info) << "-> Parsing Tag mappings";
	registerKeys();
	parseFile(std::filesystem::path("configurables/tag_mappings.txt"));
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << theMappings.size() << " mappings loaded.";
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

	// The pope don't use proper title and isn't registered.
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
			return std::move(generatedTag);
		}
	}

	// look up register
	const auto& registerItr = registeredTitleTags.find(ck3Title);
	if (registerItr != registeredTitleTags.end())
		return registerItr->second;

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
	return std::move(generatedTag);
}

std::string mappers::TitleTagMapper::generateNewTag()
{
	std::ostringstream generatedEU4TagStream;
	generatedEU4TagStream << generatedEU4TagPrefix << generatedEU4TagMidfix << generatedEU4TagSuffix;
	const auto& eu4Tag = generatedEU4TagStream.str();
	const std::set<char> reservedPrefixes = {
		 'C',
		 'D',
		 'E',
		 'F',
		 'K',
		 'O',
		 'S',
		 'T',
	};

	++generatedEU4TagSuffix;
	if (generatedEU4TagSuffix == ':') // :, 1 past 9
		generatedEU4TagSuffix = 'A';	 // jump to A, go on to Z.
	if (generatedEU4TagSuffix == '[') // [, 1 past Z
	{
		generatedEU4TagSuffix = '0';
		++generatedEU4TagMidfix;
		if (generatedEU4TagMidfix == ':')
		{
			generatedEU4TagMidfix = '0';
			--generatedEU4TagPrefix;
			while (reservedPrefixes.contains(generatedEU4TagPrefix))
				--generatedEU4TagPrefix;
		}
	}
	return eu4Tag;
}
