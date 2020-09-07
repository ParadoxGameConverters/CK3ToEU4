#include "CoatOfArms.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::CoatOfArms::CoatOfArms(std::istream& theStream, int ID): ID(ID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::CoatOfArms::registerKeys()
{
	registerKeyword("pattern", [this](const std::string& unused, std::istream& theStream) {
		pattern = commonItems::singleString(theStream).getString();
	});
	registerKeyword("color1", [this](const std::string& unused, std::istream& theStream) {
		color1 = laFabricaDeColor.getColor(theStream);
	});
	registerKeyword("color2", [this](const std::string& unused, std::istream& theStream) {
		color2 = laFabricaDeColor.getColor(theStream);
	});
	registerKeyword("color3", [this](const std::string& unused, std::istream& theStream) {
		color3 = laFabricaDeColor.getColor(theStream);
	});
	registerKeyword("color4", [this](const std::string& unused, std::istream& theStream) {
		color4 = laFabricaDeColor.getColor(theStream);
	});
	registerKeyword("color5", [this](const std::string& unused, std::istream& theStream) {
		color5 = laFabricaDeColor.getColor(theStream);
	});
	registerKeyword("textured_emblem", [this](const std::string& unused, std::istream& theStream) {
		texturedEmblems.emplace_back(Emblem(theStream));
	});
	registerKeyword("colored_emblem", [this](const std::string& unused, std::istream& theStream) {
		coloredEmblems.emplace_back(Emblem(theStream));
	});
	registerKeyword("sub", [this](const std::string& unused, std::istream& theStream) {
		subs.emplace_back(std::make_shared<CoatOfArms>(theStream, ID));
	});
	registerKeyword("instance", [this](const std::string& unused, std::istream& theStream) {
		instances.emplace_back(EmblemInstance(theStream));
	});
	registerKeyword("parent", [this](const std::string& unused, std::istream& theStream) {
		parent = std::make_pair(commonItems::singleString(theStream).getString(), nullptr);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
