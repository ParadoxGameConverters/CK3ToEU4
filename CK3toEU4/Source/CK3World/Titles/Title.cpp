#include "Title.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Title::Title(std::istream& theStream, int ID): ID(ID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Title::registerKeys()
{
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
