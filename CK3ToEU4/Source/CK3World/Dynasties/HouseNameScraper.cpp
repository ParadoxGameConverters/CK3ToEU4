#include "HouseNameScraper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void CK3::HouseNameScraper::loadHouseDetails(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::HouseNameScraper::loadHouseDetails(const std::string& filename)
{
	registerKeys();
	parseFile(filename);
	clearRegisteredKeywords();
}

void CK3::HouseNameScraper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& houseKey, std::istream& theStream) {
		const auto houseDetails = HouseNameScraping(theStream);
		houseNames[houseKey] = houseDetails;
	});
}

std::optional<std::string> CK3::HouseNameScraper::getNameForKey(const std::string& houseKey) const
{
	if (houseNames.contains(houseKey))
		return houseNames.at(houseKey).getName();
	return std::nullopt;
}
std::optional<std::string> CK3::HouseNameScraper::getPrefixForKey(const std::string& houseKey) const
{
	if (houseNames.contains(houseKey))
		return houseNames.at(houseKey).getPrefix();
	return std::nullopt;
}
