#include "CountyDetails.h"
#include "../Cultures/Cultures.h"
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

void CK3::CountyDetails::loadCultures(const Cultures& cultures)
{
	auto counter = 0;
	const auto& cultureData = cultures.getCultures();
	for (const auto& countyDetail: countyDetails)
	{
		const auto& cultureDataItr = cultureData.find(countyDetail.second->getCulture().first);
		if (cultureDataItr != cultureData.end())
		{
			countyDetail.second->loadCulture(*cultureDataItr);
			++counter;
		}
		else
		{
			throw std::runtime_error(
				 "County " + countyDetail.first + " has culture " + std::to_string(countyDetail.second->getCulture().first) + "which has no definition!");
		}
	}
	Log(LogLevel::Info) << "-> " << counter << " Counties updated.";
}
