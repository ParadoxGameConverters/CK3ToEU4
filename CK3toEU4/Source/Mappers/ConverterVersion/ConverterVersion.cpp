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
	registerSetter("name", name);
	registerSetter("version", version);
	registerSetter("source", source);
	registerSetter("target", target);
	registerKeyword("minSource", [this](std::istream& theStream) {
		const auto tempString = commonItems::getString(theStream);
		minSource = GameVersion(tempString);
	});
	registerKeyword("maxSource", [this](std::istream& theStream) {
		const auto tempString = commonItems::getString(theStream);
		maxSource = GameVersion(tempString);
	});
	registerKeyword("minTarget", [this](std::istream& theStream) {
		const auto tempString = commonItems::getString(theStream);
		minTarget = GameVersion(tempString);
	});
	registerKeyword("maxTarget", [this](std::istream& theStream) {
		const auto tempString = commonItems::getString(theStream);
		maxTarget = GameVersion(tempString);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::string mappers::ConverterVersion::getDescription() const
{
	auto toReturn = "Compatible with " + source + " [v" + minSource.toShortString();
	if (maxSource != minSource)
		toReturn += "-v" + maxSource.toShortString();
	toReturn += "] and " + target + " [v" + minTarget.toShortString();
	if (maxTarget != minTarget)
		toReturn += "-v" + maxTarget.toShortString();
	toReturn += "]";

	return toReturn;
}
