#include "Dynasty.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Dynasty::Dynasty(std::istream& theStream, int theDynID): dynID(theDynID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Dynasty::registerKeys()
{
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
