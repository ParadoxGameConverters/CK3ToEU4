#include "Country.h"
#include "../../CK3World/Characters/Character.h"
#include "../../CK3World/Titles/Title.h"
#include "../../Mappers/CultureMapper/CultureMapper.h"
#include "../../Mappers/GovernmentsMapper/GovernmentsMapper.h"
#include "../../Mappers/ProvinceMapper/ProvinceMapper.h"
#include "../../Mappers/ReligionMapper/ReligionMapper.h"
#include "../../Mappers/RulerPersonalitiesMapper/RulerPersonalitiesMapper.h"
#include "Log.h"

EU4::Country::Country(std::string theTag, const std::string& filePath): tag(std::move(theTag))
{
	// Load from a country file, if one exists. Otherwise rely on defaults.
	const auto startPos = filePath.find("/countries");
	commonCountryFile = filePath.substr(startPos + 1, filePath.length() - startPos);
	details = CountryDetails(filePath);

	// We also must set a dummy history filepath for those countries that don't actually have a history file.
	const auto lastslash = filePath.find_last_of('/');
	const auto rawname = filePath.substr(lastslash + 1, filePath.length());

	historyCountryFile = "history/countries/" + tag + " - " + rawname;
}

void EU4::Country::loadHistory(const std::string& filePath)
{
	const auto startPos = filePath.find("/history");
	historyCountryFile = filePath.substr(startPos + 1, filePath.length() - startPos);
	details.parseHistory(filePath);
}

void EU4::Country::initializeFromTitle(const std::string& theTag,
	 const std::pair<std::string, std::shared_ptr<CK3::Title>>& theTitle,
	 const mappers::GovernmentsMapper& governmentsMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const mappers::CultureMapper& cultureMapper,
	 const mappers::ProvinceMapper& provinceMapper,
	 const mappers::LocalizationMapper& localizationMapper,
	 const mappers::RulerPersonalitiesMapper& rulerPersonalitiesMapper,
	 date theConversionDate)
{
	tag = theTag;
	conversionDate = theConversionDate;
	title = theTitle;
	if (commonCountryFile.empty())
		commonCountryFile = "countries/" + title->first + ".txt";
	if (historyCountryFile.empty())
		historyCountryFile = "history/countries/" + tag + " - " + title->first + ".txt";

	Log(LogLevel::Debug) << "importing " << tag << " from " << title->first;
	details.holder = title->second->getHolder()->second;
	if (details.holder->getHouse().first)
		details.house = details.holder->getHouse().second;
	else
		Log(LogLevel::Warning) << tag << " holder " << details.holder->getName() << " has no house.";	
}
