#include "Cultures.h"
#include "Culture.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Cultures::Cultures(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Cultures::registerKeys()
{
	registerRegex(R"(\d+)", [this](const std::string& faithID, std::istream& theStream) {
		auto newCulture = std::make_shared<Culture>(theStream, std::stoll(faithID));
		cultures.insert(std::pair(newCulture->getID(), newCulture));
	});
	registerKeyword("cultures", [this](const std::string& faithID, std::istream& theStream) {
		const auto scraper = Cultures(theStream);
		cultures = scraper.getCultures();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
