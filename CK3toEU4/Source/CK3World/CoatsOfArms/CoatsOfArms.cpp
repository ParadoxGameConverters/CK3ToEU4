#include "CoatsOfArms.h"
#include "CoatOfArms.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::CoatsOfArms::CoatsOfArms(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::CoatsOfArms::registerKeys()
{
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
