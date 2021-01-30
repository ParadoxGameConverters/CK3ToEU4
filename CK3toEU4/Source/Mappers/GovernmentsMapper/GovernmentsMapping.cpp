#include "GovernmentsMapping.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::GovernmentsMapping::GovernmentsMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::GovernmentsMapping::registerKeys()
{
	registerKeyword("gov", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString govStr(theStream);
		government = govStr.getString();
	});
	registerKeyword("reform", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString reformStr(theStream);
		reform = reformStr.getString();
	});
	registerKeyword("ck3title", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString ck3titleStr(theStream);
		ck3title = ck3titleStr.getString();
	});
	registerKeyword("ck3gov", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString ck3govStr(theStream);
		ck3governments.insert(ck3govStr.getString());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

bool mappers::GovernmentsMapping::matchGovernment(const std::string& ck3Government, const std::string& incTitle) const
{
	if (!ck3title.empty() && ck3title != incTitle)
		return false; // these are reserved for their recipients only.
	if (!ck3title.empty() && ck3title == incTitle)
		return true;
	if (ck3governments.count(ck3Government))
		return true;
	return false;
}