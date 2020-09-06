#include "Emblem.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Emblem::Emblem(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Emblem::registerKeys()
{
	registerKeyword("texture", [this](const std::string& unused, std::istream& theStream) {
		texture = commonItems::singleString(theStream).getString();
	});
	registerKeyword("color1", [this](const std::string& unused, std::istream& theStream) {
		color1 = commonItems::singleString(theStream).getString();
	});
	registerKeyword("color2", [this](const std::string& unused, std::istream& theStream) {
		color2 = commonItems::singleString(theStream).getString();
	});
	registerKeyword("color3", [this](const std::string& unused, std::istream& theStream) {
		color3 = commonItems::singleString(theStream).getString();
	});
	registerKeyword("mask", [this](const std::string& unused, std::istream& theStream) {
		mask = commonItems::intList(theStream).getInts();
	});
	registerKeyword("instance", [this](const std::string& unused, std::istream& theStream) {
		instances.emplace_back(EmblemInstance(theStream));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
