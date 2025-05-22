#include "AfricanPassesMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::AfricanPassesMapper::AfricanPassesMapper()
{
	Log(LogLevel::Info) << "-> Parsing African Passes.";
	registerKeys();
	parseFile(std::filesystem::path("configurables/african_passes.txt"));
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << passes.size() << " passes.";
}

mappers::AfricanPassesMapper::AfricanPassesMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::AfricanPassesMapper::registerKeys()
{
	registerKeyword("pass", [this](std::istream& theStream) {
		const AfricanPassesMapping pass(theStream);
		passes.emplace_back(pass);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
