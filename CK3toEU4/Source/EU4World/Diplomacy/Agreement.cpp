#include "Agreement.h"
#include "ParserHelpers.h"

EU4::Agreement::Agreement(std::istream& theStream, std::string theType): type(std::move(theType))
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::Agreement::registerKeys()
{
	registerKeyword("first", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString firstStr(theStream);
		first = firstStr.getString();
	});
	registerKeyword("second", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString secondStr(theStream);
		second = secondStr.getString();
	});
	registerKeyword("subject_type", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString typeStr(theStream);
		subjectType = typeStr.getString();
	});
	registerKeyword("start_date", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString dateStr(theStream);
		startDate = date(dateStr.getString());
	});
	registerKeyword("end_date", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString dateStr(theStream);
		endDate = date(dateStr.getString());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void EU4::Agreement::updateTags(const std::string& oldTag, const std::string& newTag)
{
	if (first == oldTag)
		first = newTag;
	if (second == oldTag)
		second = newTag;
}
