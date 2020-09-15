#include "Country.h"
#include "../../CK3World/Characters/Character.h"
#include "../../CK3World/Titles/Title.h"
#include "../../Mappers/CultureMapper/CultureMapper.h"
#include "../../Mappers/GovernmentsMapper/GovernmentsMapper.h"
#include "../../Mappers/ProvinceMapper/ProvinceMapper.h"
#include "../../Mappers/ReligionMapper/ReligionMapper.h"
#include "../../Mappers/RulerPersonalitiesMapper/RulerPersonalitiesMapper.h"
#include "../../CK3World/Religions/Faith.h"
#include "../../CK3World/Religions/Religion.h"
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

	populateHistory(governmentsMapper, religionMapper, provinceMapper);
	
}

void EU4::Country::populateHistory(const mappers::GovernmentsMapper& governmentsMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const mappers::ProvinceMapper& provinceMapper)
{
	// --------------- History section
	details.government.clear();
	details.reforms.clear();
	const auto& newGovernment = governmentsMapper.matchGovernment(details.holder->getDomain()->getGovernment(), title->first);
	if (newGovernment)
	{
		details.government = newGovernment->first;
		if (!newGovernment->second.empty())
			details.reforms.insert(newGovernment->second);
	}
	else
	{
		Log(LogLevel::Warning) << "No government match for " << details.holder->getDomain()->getGovernment() << " for title: " << title->first
									  << ", defaulting to monarchy.";
		details.government = "monarchy";
	}
	if (title->second->getLevel() == CK3::LEVEL::EMPIRE)
		details.governmentRank = 3;
	else if (title->second->getLevel() == CK3::LEVEL::KINGDOM)
		details.governmentRank = 2;
	else
		details.governmentRank = 1;
	// Reforms will be set later to ensure that all other aspects of a country have been correctly set first.

	const std::string baseReligion = details.holder->getFaith().second->getReligion().second->getName();
	const auto& religionMatch = religionMapper.getEU4ReligionForCK3Religion(baseReligion);
	if (religionMatch)
		details.religion = *religionMatch;
	else
	{
		// We failed to get a religion. This is not an issue. We'll set it later from the majority of owned provinces.
		Log(LogLevel::Warning) << tag << " has no match for base religion: " << baseReligion;
		details.religion.clear();
	}
	// Change capitals for anyone not aztec.
	if (tag != "AZT")
	{
		const auto& capitalMatch = provinceMapper.getEU4ProvinceNumbers(actualHolder->getCapitalProvince().first);
		if (!capitalMatch.empty())
			details.capital = *capitalMatch.begin();
		else
			details.capital = 0; // We will see warning about this earlier, no need for more spam.
	}
	// do we have a culture? Pope is special as always.
	std::string baseCulture;
	if (title.second->isThePope() || title.second->isTheFraticelliPope())
		baseCulture = actualHolder->getCapitalProvince().second->getCulture();
	else if (!actualHolder->getCulture().empty())
		baseCulture = actualHolder->getCulture();
	else
		baseCulture = actualHolder->getCapitalProvince().second->getCulture();
	const auto& cultureMatch = cultureMapper.cultureMatch(baseCulture, details.religion, details.capital, tag);
	if (cultureMatch)
		details.primaryCulture = *cultureMatch;
	else
	{
		// We failed to get a primaryCulture. This is not an issue. We'll set it later from the majority of owned provinces.
		details.primaryCulture.clear();
	}
	if (!details.primaryCulture.empty())
	{
		const auto& techMatch = cultureMapper.getTechGroup(details.primaryCulture);
		if (techMatch)
			details.technologyGroup = *techMatch;
	} // We will set it later if primaryCulture is unavailable at this stage.
	if (title.first.find("c_") == 0)
		details.fixedCapital = true;
	else
		details.fixedCapital = false;
	if (details.reforms.count("merchants_reform"))
		details.mercantilism = 25; // simulates merchant power of merchant republics.
	else
		details.mercantilism = 0;
	// Unit type should automatically match tech group. If not we'll add logic for it here.
	details.unitType.clear();
	// religious_school can be picked by player at leisure.
	details.religiousSchool.clear();
	// ditto for cults
	details.cults.clear();
	// We fill accepted cultures later, manually, once we can do a provincial census
	details.acceptedCultures.clear();
	if (details.reforms.count("monastic_order_reform"))
		details.armyProfessionalism = 0.05;
	else
		details.armyProfessionalism = 0;
	// We don't do historical rivalries or friendships at this stage.
	details.historicalRivals.clear();
	details.historicalFriends.clear();
	// We don't do national focus at this stage.
	details.nationalFocus.clear();
	// Piety is related to religious_school, which we don't set.
	details.piety = 0;
	// HRE Electorate is set later, once we can do a province/dev check.
	details.elector = false;
	if (title.second->isHREEmperor())
		details.holyRomanEmperor = true;
	if (title.second->isInHRE())
		details.inHRE = true;
	// ditto for secondary_religion and harmonized religions.
	details.secondaryReligion.clear();
	details.harmonizedReligions.clear();
	details.historicalScore = 0; // Not sure about this.
}
