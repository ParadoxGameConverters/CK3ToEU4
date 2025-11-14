#include "LandedTitles.h"
#include "../Geography/CountyDetails.h"
#include "../Geography/ProvinceHoldings.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "Title.h"
#include "Titles.h"

// This is a recursive class that scrapes 00_landed_titles.txt (and related files) looking for title colors, landlessness,
// and most importantly relation between baronies and barony provinces so we can link titles to actual clay.
// Since titles are nested according to hierarchy we do this recursively.

// Keep in mind that we use this class as middleware between titles and hard geographical data. Baronies and Counties have said data,
// but newfangled custom empires and such found in Titles will not be present here. They should have colors defined in their Title block
// anyway, and whatever relates to Title over there takes precedence over data in this class.

void CK3::LandedTitles::loadTitles(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::LandedTitles::loadTitles(const std::filesystem::path& fileName)
{
	registerKeys();
	parseFile(fileName);
	clearRegisteredKeywords();
}

void CK3::LandedTitles::registerKeys()
{
	registerRegex(R"((h|e|k|d|c|b)_[A-Za-z0-9_\-\']+)", [this](const std::string& titleName, std::istream& theStream) {
		// Pull the titles beneath this one and add them to the lot, overwriting existing ones.
		auto newTitle = std::make_shared<LandedTitles>();
		newTitle->loadTitles(theStream);
		for (const auto& locatedTitle: newTitle->getFoundTitles())
			foundTitles[locatedTitle.first] = locatedTitle.second;

		// And then add this one as well, overwriting existing.
		foundTitles[titleName] = newTitle;
	});
	registerKeyword("definite_form", [this](const std::string& unused, std::istream& theStream) {
		definiteForm = commonItems::singleString(theStream).getString() == "yes";
	});
	registerKeyword("landless", [this](const std::string& unused, std::istream& theStream) {
		landless = commonItems::singleString(theStream).getString() == "yes";
	});
	registerKeyword("color", [this](const std::string& unused, std::istream& theStream) {
		try
		{
			color = laFabricaDeColor.getColor(theStream);
		}
		catch (std::exception& e)
		{
			Log(LogLevel::Error) << "Color loading failure: " << e.what() << " ... Replacing with grey.";
			color = commonItems::Color(std::array<int, 3>{100, 100, 100});
		}
	});
	registerKeyword("capital", [this](const std::string& unused, std::istream& theStream) {
		capital = std::make_pair(commonItems::singleString(theStream).getString(), nullptr);
	});
	registerKeyword("province", [this](const std::string& unused, std::istream& theStream) {
		province = std::make_pair(commonItems::singleInt(theStream).getInt(), nullptr);
	});
	registerKeyword("can_be_named_after_dynasty", [this](const std::string& unused, std::istream& theStream) {
		mayBeNamedAfterDynasty = commonItems::singleString(theStream).getString() == "yes";
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void CK3::LandedTitles::linkProvinceHoldings(const ProvinceHoldings& provinceHoldings)
{
	// We're linking provinces into BARONY titles only, as other titles have no holding data.
	auto counter = 0;
	const auto& provinceData = provinceHoldings.getProvinceHoldings();
	for (const auto& landedTitle: foundTitles)
	{
		if (landedTitle.first.find("b_") != 0)
			continue;
		if (!landedTitle.second->getProvince())
			throw std::runtime_error("Landed title " + landedTitle.first + " has not province holding defined!");

		const auto& provinceDataItr = provinceData.find(landedTitle.second->getProvince()->first);
		if (provinceDataItr != provinceData.end())
		{
			landedTitle.second->loadProvinceHolding(*provinceDataItr);
			++counter;
		}
		else
		{
			throw std::runtime_error("Landed title " + landedTitle.first + " has province holding " + std::to_string(landedTitle.second->getProvince()->first) +
											 " which has no definition!");
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " landed titles updated.";
}

void CK3::LandedTitles::linkCountyDetails(const CountyDetails& countyDetails)
{
	// We're linking county details into COUNTY titles only, as other titles have no such details.
	auto counter = 0;
	const auto& countyData = countyDetails.getCountyDetails();
	for (const auto& landedTitle: foundTitles)
	{
		if (landedTitle.first.find("c_") != 0)
			continue;
		const auto& countyDataItr = countyData.find(landedTitle.first);
		if (countyDataItr != countyData.end())
		{
			landedTitle.second->loadCountyDetails(*countyDataItr);
			++counter;
		}
		else if (landedTitle.first.find("c_nf_") == 0)
		{
			// This is a virtual county-level title for daimyos and whatnot. Ignore this issue.
		}
		else
		{
			// Nothing. People with missing titles from mods and whatnot can have holes on their map.
			Log(LogLevel::Warning) << "Landed title " << landedTitle.first << " has no definition in counties={} section of the save game!";
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " landed titles updated.";
}

void CK3::LandedTitles::linkTitles(const Titles& titles)
{
	auto counter = 0;
	const auto& titleData = titles.getTitles();
	for (const auto& landedTitle: foundTitles)
	{
		const auto& theCapital = landedTitle.second->getCapital();
		if (!theCapital)
			continue;
		if (const auto& titleDataItr = titleData.find(theCapital->first); titleDataItr != titleData.end())
		{
			landedTitle.second->loadCapital(*titleDataItr);
			++counter;
		}
		else
		{
			Log(LogLevel::Error) << "Landed title " + landedTitle.first + " has a capital " + theCapital->first + " which has no definition!";
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " landed title capitals updated.";
}
