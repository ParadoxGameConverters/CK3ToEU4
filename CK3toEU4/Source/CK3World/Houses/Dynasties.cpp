#include "Dynasties.h"
#include "Dynasty.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Dynasties::Dynasties(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Dynasties::registerKeys()
{
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}