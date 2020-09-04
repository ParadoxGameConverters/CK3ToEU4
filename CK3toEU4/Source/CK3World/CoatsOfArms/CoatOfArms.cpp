#include "CoatOfArms.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::CoatOfArms::CoatOfArms(std::istream& theStream, int ID): ID(ID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::CoatOfArms::registerKeys()
{
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
