#include "VersionParser.h"
#include "ParserHelpers.h"



mappers::VersionParser::VersionParser()
{
	registerKeys();
	parseFile("DataFiles/version.txt");
	clearRegisteredKeywords();
}


mappers::VersionParser::VersionParser(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}


void mappers::VersionParser::registerKeys()
{
	registerKeyword("name", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString nameStr(theStream);
		name = nameStr.getString();
	});
	registerKeyword("version", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString versionStr(theStream);
		version = versionStr.getString();
	});
	registerKeyword("descriptionLine", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString descriptionLineStr(theStream);
		descriptionLine = descriptionLineStr.getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}