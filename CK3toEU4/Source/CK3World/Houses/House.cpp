#include "House.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::House::House(std::istream& theStream, int houseID): houseID(houseID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::House::registerKeys()
{
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
