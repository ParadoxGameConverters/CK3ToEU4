#include "CountyDetails.h"
#include "../Cultures/Cultures.h"
#include "../Religions/Faiths.h"
#include "CommonRegexes.h"
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

void CK3::CountyDetails::linkCultures(const Cultures& cultures)
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
				 "County " + countyDetail.first + " has culture " + std::to_string(countyDetail.second->getCulture().first) + " which has no definition!");
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " counties updated.";
}

void CK3::CountyDetails::linkFaiths(const Faiths& faiths)
{
	auto counter = 0;
	const auto& faithData = faiths.getFaiths();
	for (const auto& countyDetail: countyDetails)
	{
		const auto& faithDataItr = faithData.find(countyDetail.second->getFaith().first);
		if (faithDataItr != faithData.end())
		{
			countyDetail.second->loadFaith(*faithDataItr);
			++counter;
		}
		else
		{
			throw std::runtime_error(
				 "County " + countyDetail.first + " has faith " + std::to_string(countyDetail.second->getFaith().first) + " which has no definition!");
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " counties updated.";
}
