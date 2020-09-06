#include "Religions.h"
#include "Religion.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Religions::Religions(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Religions::registerKeys()
{
	registerRegex(R"(\d+)", [this](const std::string& faithID, std::istream& theStream) {
		auto newReligion = std::make_shared<Religion>(theStream, std::stoi(faithID));
		religions.insert(std::pair(newReligion->getID(), newReligion));
	});
	registerKeyword("religions", [this](const std::string& unused, std::istream& theStream) {
		religions = Religions(theStream).getReligions();
	});
	registerKeyword("faiths", [this](const std::string& unused, std::istream& theStream) {
		faiths = Faiths(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
