#include "ShatterEmpiresMapper.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::ShatterEmpiresMapper::ShatterEmpiresMapper()
{
	registerKeys();
	parseFile(std::filesystem::path("configurables/shatter_empires.txt"));
	clearRegisteredKeywords();
}

mappers::ShatterEmpiresMapper::ShatterEmpiresMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ShatterEmpiresMapper::registerKeys()
{
	registerKeyword("shatter_empires", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::stringList empList(theStream);
		auto emps = empList.getStrings();
		empires.insert(emps.begin(), emps.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}