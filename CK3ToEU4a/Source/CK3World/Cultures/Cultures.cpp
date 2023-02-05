#include "Cultures.h"
#include "CommonRegexes.h"
#include "Culture.h"
#include "Log.h"
#include "ParserHelpers.h"
#include <ranges>

CK3::Cultures::Cultures(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Cultures::registerKeys()
{
	registerRegex(R"(\d+)", [this](const std::string& cultureID, std::istream& theStream) {
		auto newCulture = std::make_shared<Culture>(theStream, std::stoll(cultureID));
		cultures.insert(std::pair(newCulture->getID(), newCulture));
	});
	registerKeyword("cultures", [this](std::istream& theStream) {
		const auto scraper = Cultures(theStream);
		cultures = scraper.getCultures();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void CK3::Cultures::concoctCultures(const mappers::LocalizationMapper& localizationMapper, const mappers::CultureMapper& cultureMapper)
{
	for (const auto& culture: cultures | std::views::values)
	{
		culture->concoctCultureName(localizationMapper, cultureMapper, cultureNamingCounter);
	}
}
