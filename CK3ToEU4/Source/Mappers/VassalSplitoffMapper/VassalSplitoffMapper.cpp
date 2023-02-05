#include "VassalSplitoffMapper.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::VassalSplitoffMapper::VassalSplitoffMapper()
{
	registerKeys();
	parseFile("configurables/vassal_splitoff.txt");
	clearRegisteredKeywords();
}

mappers::VassalSplitoffMapper::VassalSplitoffMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::VassalSplitoffMapper::registerKeys()
{
	registerSetter("sensitivity", sensitivity);
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

double mappers::VassalSplitoffMapper::getFactor() const
{
	if (sensitivity > 1 && sensitivity < 1000)
		return 100.0 / static_cast<double>(sensitivity);
	else
		return 1;
}
