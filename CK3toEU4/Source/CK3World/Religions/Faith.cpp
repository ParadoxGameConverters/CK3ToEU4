#include "Faith.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Faith::Faith(std::istream& theStream, int ID): ID(ID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Faith::registerKeys()
{
	registerKeyword("tag", [this](const std::string& unused, std::istream& theStream) {
		tag = commonItems::singleString(theStream).getString();
	});
	registerKeyword("doctrine", [this](const std::string& unused, std::istream& theStream) {
		doctrines.insert(commonItems::singleString(theStream).getString());
	});
	registerKeyword("religion", [this](const std::string& unused, std::istream& theStream) {
		religion = std::make_pair(commonItems::singleInt(theStream).getInt(), nullptr);
	});
	registerKeyword("color", [this](const std::string& unused, std::istream& theStream) {
		color = commonItems::Color::Factory().getColor(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
