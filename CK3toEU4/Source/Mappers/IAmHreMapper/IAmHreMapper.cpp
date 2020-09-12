#include "IAmHreMapper.h"
#include "ParserHelpers.h"

mappers::IAmHreMapper::IAmHreMapper()
{
	registerKeys();
	parseFile("configurables/i_am_hre.txt");
	clearRegisteredKeywords();
}

mappers::IAmHreMapper::IAmHreMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::IAmHreMapper::registerKeys()
{
	registerKeyword("hre_mechanics", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString hreStr(theStream);
		hre = hreStr.getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}