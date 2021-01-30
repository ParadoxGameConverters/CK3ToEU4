#include "ConverterVersion.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::ConverterVersion::ConverterVersion()
{
	registerKeys();
	parseFile("configurables/version.txt");
	clearRegisteredKeywords();
}

mappers::ConverterVersion::ConverterVersion(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ConverterVersion::registerKeys()
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