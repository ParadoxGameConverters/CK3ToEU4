#include "Houses.h"
#include "House.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Houses::Houses(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Houses::registerKeys()
{
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
