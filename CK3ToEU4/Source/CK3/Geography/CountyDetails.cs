
using CK3;

namespace CK3ToEU4.CK3.Geography;

class CountyDetails
{
	public CountyDetails()
	{
	}

	public CountyDetails(BufferedReader reader)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}

	public  const auto& getCountyDetails() const { return countyDetails; }

	public void linkCultures(const Cultures& cultures)
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
				throw new Exception(
					"County " + countyDetail.first + " has culture " + std::to_string(countyDetail.second->getCulture().first) + " which has no definition!");
			}
		}
		Log(LogLevel::Info) << "<> " << counter << " counties updated.";
	}
	public void linkFaiths(const Faiths& faiths)
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
				throw new Exception(
					"County " + countyDetail.first + " has faith " + std::to_string(countyDetail.second->getFaith().first) + " which has no definition!");
			}
		}
		Log(LogLevel::Info) << "<> " << counter << " counties updated.";
	}

	
	private void registerKeys()
	{
		registerRegex(R"(c_[A-Za-z0-9_\-\']+)", [this](const string& countyName, std::istream& theStream) {
			auto newCounty = std::make_shared<CountyDetail>(theStream);
			countyDetails.insert(std::pair(countyName, newCounty));
		});
		registerKeyword("counties", [this](const string& countyName, std::istream& theStream) {
			const auto scraper = CountyDetails(theStream);
			countyDetails = scraper.getCountyDetails();
		});
		registerRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}

	private Dictionary<string, std::shared_ptr<CountyDetail>> countyDetails;
};