#include "CountyDetails.h"
#include "CountyDetail.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::CountyDetails::CountyDetails(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::CountyDetails::registerKeys()
{
	registerRegex(R"(c_[A-Za-z0-9_\-\']+)", [this](const std::string& countyName, std::istream& theStream) {
		auto newCounty = std::make_shared<CountyDetail>(theStream);
		countyDetails.insert(std::pair(countyName, newCounty));
	});
	registerKeyword("counties", [this](const std::string& countyName, std::istream& theStream) {
		const auto scraper = CountyDetails(theStream);
		countyDetails = scraper.getCountyDetails();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
