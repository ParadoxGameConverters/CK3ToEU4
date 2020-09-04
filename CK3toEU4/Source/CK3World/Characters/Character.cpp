#include "Character.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Character::Character(std::istream& theStream, int chrID): charID(chrID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Character::registerKeys()
{
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
