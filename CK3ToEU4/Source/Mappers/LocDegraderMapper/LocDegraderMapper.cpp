#include "LocDegraderMapper.h"
#include "CommonRegexes.h"
#include "LocDegraderMapping.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::LocDegraderMapper::LocDegraderMapper()
{
	Log(LogLevel::Info) << "-> Parsing character replacement guidelines.";
	registerKeys();
	parseFile("configurables/char_reduce.txt");
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << replacements.size() << " character alternatives.";
}

mappers::LocDegraderMapper::LocDegraderMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::LocDegraderMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		const LocDegraderMapping newMapping(theStream);
		if (!newMapping.getReplacementMapping().first.empty() && !newMapping.getReplacementMapping().second.empty())
			replacements.emplace(newMapping.getReplacementMapping());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::string mappers::LocDegraderMapper::degradeString(const std::string& inputString) const
{
	std::string toReturn = inputString;
	for (const auto& [utf8, win1252]: replacements)
	{
		toReturn = std::regex_replace(toReturn, std::regex(utf8), win1252);
	}
	return toReturn;
}
