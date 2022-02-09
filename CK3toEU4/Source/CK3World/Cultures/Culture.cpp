#include "Culture.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Culture::Culture(std::istream& theStream, long long theID): ID(theID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Culture::registerKeys()
{
	registerKeyword("culture_template", [this](std::istream& theStream) {
		canonical = true;
		culture_template = commonItems::getString(theStream);
	});
	registerKeyword("name", [this](std::istream& theStream) {
		name = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::string CK3::Culture::getName() const
{
	if (!culture_template.empty())
		return culture_template; // this is the canonical culture name ie. "swedish"

	if (!name.empty())
	{
		std::string toReturn = name;
		// converting anything to lowercase may be impossible as we may have chinese coming in. We can guesstimate by first letter though?
		if (name[0] >= 'A' && name[0] <= 'Z')
		{
			toReturn[0] = ::tolower(name[0]);
			return toReturn; // this can be anything.
		}
	}
	return "NODATAMALFORMED";
}
