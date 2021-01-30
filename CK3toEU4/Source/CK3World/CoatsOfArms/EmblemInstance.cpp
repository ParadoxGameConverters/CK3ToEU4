#include "EmblemInstance.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

CK3::EmblemInstance::EmblemInstance(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::EmblemInstance::registerKeys()
{
	registerKeyword("rotation", [this](const std::string& unused, std::istream& theStream) {
		rotation = commonItems::singleDouble(theStream).getDouble();
	});
	registerKeyword("depth", [this](const std::string& unused, std::istream& theStream) {
		depth = commonItems::singleDouble(theStream).getDouble();
	});
	registerKeyword("position", [this](const std::string& unused, std::istream& theStream) {
		position = commonItems::doubleList(theStream).getDoubles();
	});
	registerKeyword("scale", [this](const std::string& unused, std::istream& theStream) {
		scale = commonItems::doubleList(theStream).getDoubles();
	});
	registerKeyword("offset", [this](const std::string& unused, std::istream& theStream) {
		offset = commonItems::doubleList(theStream).getDoubles();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
