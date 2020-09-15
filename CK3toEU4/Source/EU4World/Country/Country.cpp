#include "Country.h"
#include "../../CK3World/Characters/Character.h"
#include "../../CK3World/Cultures/Culture.h"
#include "../../CK3World/Geography/CountyDetail.h"
#include "../../CK3World/Religions/Faith.h"
#include "../../CK3World/Titles/LandedTitles.h"
#include "../../CK3World/Titles/Title.h"
#include "../../Mappers/CultureMapper/CultureMapper.h"
#include "../../Mappers/GovernmentsMapper/GovernmentsMapper.h"
#include "../../Mappers/ProvinceMapper/ProvinceMapper.h"
#include "../../Mappers/ReligionMapper/ReligionMapper.h"
#include "../../Mappers/RulerPersonalitiesMapper/RulerPersonalitiesMapper.h"
#include "Log.h"
#include <cmath>

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

	populateHistory(governmentsMapper, religionMapper, provinceMapper, cultureMapper);
	populateCommons(cultureMapper);
	populateMisc();
}

void EU4::Country::populateHistory(const mappers::GovernmentsMapper& governmentsMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const mappers::ProvinceMapper& provinceMapper,
	 const mappers::CultureMapper& cultureMapper)
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

	const std::string baseReligion = details.holder->getFaith().second->getName();
	const auto& religionMatch = religionMapper.getEU4ReligionForCK3Religion(baseReligion);
	if (religionMatch)
		details.religion = *religionMatch;
	else
	{
		// We failed to get a religion. This is not an issue. We'll set it later from the majority of owned provinces.
		Log(LogLevel::Warning) << tag << " has no match for base faith: " << baseReligion;
		details.religion.clear();
	}
	// Change capitals for anyone not aztec.
	if (tag != "AZT")
	{
		const auto& capitalMatch = provinceMapper.getEU4ProvinceNumbers(details.holder->getDomain()->getRealmCapital().second->getDJLiege()->second->getName());
		if (!capitalMatch.empty())
		{
			details.capital = *capitalMatch.begin();
		}
		else
		{
			Log(LogLevel::Debug) << "No match for capital: " << details.holder->getDomain()->getRealmCapital().second->getDJLiege()->second->getName();
			details.capital = 0; // We will see warning about this earlier, no need for more spam.
		}
	}
	// do we have a culture? Pope is special as always.
	std::string baseCulture;
	if (title->second->isThePope())
	{
		// This is getting absurd.
		baseCulture = details.holder->getDomain()->getRealmCapital().second->getDJLiege()->second->getClay()->getCounty()->second->getCulture().second->getName();
	}
	else
	{
		baseCulture = details.holder->getCulture().second->getName();
	}
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

	if (title->second->getLevel() == CK3::LEVEL::COUNTY)
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
	if (title->second->isHREEmperor())
		details.holyRomanEmperor = true;
	if (title->second->isInHRE())
		details.inHRE = true;
	// ditto for secondary_religion and harmonized religions.
	details.secondaryReligion.clear();
	details.harmonizedReligions.clear();
	details.historicalScore = 0; // Not sure about this.
}

void EU4::Country::populateCommons(const mappers::CultureMapper& cultureMapper)
{
	// --------------  Common section
	if (!details.primaryCulture.empty())
	{
		const auto& gfxmatch = cultureMapper.getGFX(details.primaryCulture);
		if (gfxmatch)
			details.graphicalCulture = *gfxmatch;
		else
		{
			Log(LogLevel::Warning) << tag << ": No gfx match for: " << details.primaryCulture << "! Substituting westerngfx!";
			details.graphicalCulture = "westerngfx";
		}
	} // We will set it later if primaryCulture/gfx is unavailable at this stage.
	if (!details.color)
	{
		if (title->second->getColor())
			details.color = title->second->getColor();
		else
			Log(LogLevel::Warning) << tag << ": No color defined for title " << title->first << "!";
	}

	// If we imported some revolutionary colors we'll keep them, otherwise we're ignoring this.
	// If we imported historical idea groups, keeping them, otherwise blank.
	details.randomChance = false; // random chance related to RNW, wo it has no effect here.

	// If we imported historical units, keeping them, otherwise blank.

	details.monarchNames.clear();
	if (!title->second->getPreviousHolders().empty())
	{
		for (const auto& previousHolder: title->second->getPreviousHolders())
		{
			const auto& blockItr = details.monarchNames.find(previousHolder.second->getName());
			if (blockItr != details.monarchNames.end())
				blockItr->second.first++;
			else
			{
				const auto female = previousHolder.second->isFemale();
				auto chance = 10;
				if (female)
					chance = -1;
				std::pair<int, int> newBlock = std::pair(1, chance);
				details.monarchNames.insert(std::pair(previousHolder.second->getName(), newBlock));
			}
		}
	}
	if (!title->second->getHolder()->second->getCourtierNames().empty())
	{
		for (const auto& courtier: title->second->getHolder()->second->getCourtierNames())
		{
			const auto& blockItr = details.monarchNames.find(courtier.first);
			if (blockItr == details.monarchNames.end())
			{
				const auto female = !courtier.second;
				auto chance = 0;
				if (female)
					chance = -1;
				std::pair<int, int> newBlock = std::pair(0, chance);
				details.monarchNames.insert(std::pair(courtier.first, newBlock));
			}
		}
	}

	// If we imported leader_names, keeping them, otherwise blank.
	// If we imported ship_names, keeping them, otherwise blank.
	// If we imported army_names, keeping them, otherwise blank.
	// If we imported fleet_names, keeping them, otherwise blank.
	// If we imported preferred_religion, keeping it, otherwise blank.
	// If we imported colonial_parent, keeping it, otherwise blank.
	// If we imported special_unit_culture, keeping it, otherwise blank.
	// If we imported all_your_core_are_belong_to_us, keeping it, otherwise blank.
	// If we imported right_to_bear_arms, keeping it, otherwise blank.
}

void EU4::Country::populateMisc()
{

	if (details.holder->getGold() > 0)
		details.addTreasury = lround(7 * log2(details.holder->getGold()));
	if (details.holder->getPrestige() > 0)
		details.addPrestige = -50 + std::max(-50, static_cast<int>(lround(15 * log2(details.holder->getPrestige() - 100) - 50)));
	if (details.holder->hasTrait("excommunicated"))
		details.excommunicated = true;
}
