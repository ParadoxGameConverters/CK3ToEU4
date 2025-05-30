#include "Country.h"
#include "../../CK3World/Characters/Character.h"
#include "../../CK3World/Characters/CharacterDomain.h"
#include "../../CK3World/Confederations/Confederation.h"
#include "../../CK3World/Cultures/Culture.h"
#include "../../CK3World/Dynasties/Dynasty.h"
#include "../../CK3World/Dynasties/House.h"
#include "../../CK3World/Geography/CountyDetail.h"
#include "../../CK3World/Religions/Faith.h"
#include "../../CK3World/Titles/LandedTitles.h"
#include "../../CK3World/Titles/Title.h"
#include "../../Mappers/CultureDefinitionsMapper/CultureDefinitionsMapper.h"
#include "../../Mappers/CultureDefinitionsMapper/CultureGroupDefinition.h"
#include "../../Mappers/CultureMapper/CultureMapper.h"
#include "../../Mappers/GovernmentsMapper/GovernmentsMapper.h"
#include "../../Mappers/ProvinceMapper/ProvinceMapper.h"
#include "../../Mappers/ReligionMapper/ReligionMapper.h"
#include "../../Mappers/RulerPersonalitiesMapper/RulerPersonalitiesMapper.h"
#include "../Province/EU4Province.h"
#include "CommonFunctions.h"
#include "Log.h"
#include <cmath>

EU4::Country::Country(std::string theTag, const std::filesystem::path& filePath): tag(std::move(theTag))
{
	// Load from a country file, if one exists. Otherwise rely on defaults.
	if (!commonItems::DoesFileExist(filePath))
		throw std::runtime_error("Cannot create country from: " + filePath.string());
	commonCountryFile = filePath.filename();
	details = CountryDetails(filePath);

	// We also must set a dummy history filepath for those countries that don't actually have a history file.
	historyCountryFile = tag + " - " + filePath.filename().string();
}

void EU4::Country::loadHistory(const std::filesystem::path& filePath)
{
	historyCountryFile = filePath.filename();
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
	 date theConversionDate,
	 date startDate,
	 Configuration::STARTDATE startDateOption,
	 bool dynasticNames)
{
	tag = theTag;
	if (startDateOption == Configuration::STARTDATE::CK)
		conversionDate = theConversionDate;
	else if (startDateOption == Configuration::STARTDATE::MANUAL)
		conversionDate = startDate;
	else
		conversionDate = date(1444, 11, 11);
	title = theTitle;
	if (commonCountryFile.empty())
		commonCountryFile = title->first + ".txt";
	if (historyCountryFile.empty())
		historyCountryFile = tag + " - " + title->first + ".txt";

	details.holder = title->second->getHolder()->second;
	if (details.holder->getHouse().first)
		details.house = details.holder->getHouse().second;

	populateHistory(governmentsMapper, religionMapper, provinceMapper, cultureMapper);
	populateCommons(cultureMapper, localizationMapper);
	populateMisc();
	populateLocs(localizationMapper, dynasticNames);
	populateRulers(religionMapper, cultureMapper, rulerPersonalitiesMapper, localizationMapper, startDateOption, theConversionDate, startDate);
}

void EU4::Country::populateHistory(const mappers::GovernmentsMapper& governmentsMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const mappers::ProvinceMapper& provinceMapper,
	 const mappers::CultureMapper& cultureMapper)
{
	// Are we sane?
	if (!details.holder->getCharacterDomain())
	{
		Log(LogLevel::Warning) << tag << "'s holder has no domain. Breaking history init.";
		return;
	}

	// --------------- History section
	details.government.clear();
	details.reforms.clear();
	const auto& newGovernment = governmentsMapper.matchGovernment(details.holder->getCharacterDomain()->getGovernment(), title->first);
	if (newGovernment)
	{
		details.government = newGovernment->first;
		if (!newGovernment->second.empty())
			details.reforms.insert(newGovernment->second);
	}
	else
	{
		Log(LogLevel::Warning) << "No government match for " << details.holder->getCharacterDomain()->getGovernment() << " for title: " << title->first
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

	std::string baseReligion;
	std::string baseReligiousHead;
	if (details.holder->getFaith())
	{
		baseReligion = details.holder->getFaith()->second->getName();
		baseReligiousHead = details.holder->getFaith()->second->getReligiousHead();
	}
	if (baseReligion.empty())
		Log(LogLevel::Warning) << tag << " base faith has no name!";

	// religious_school we imported needs to be cleared before we try setting it along with religion.
	details.religiousSchool.clear();
	if (const auto& religionMatch = religionMapper.getEU4ReligionForCK3Religion(baseReligion, baseReligiousHead); religionMatch)
	{
		details.religion = *religionMatch;
		if (const auto& schoolMatch = religionMapper.getEU4SchoolForCK3Religion(baseReligion); schoolMatch)
			details.religiousSchool = *schoolMatch;
	}
	else
	{
		// We failed to get a religion. This is not an issue. We'll set it later from the majority of owned provinces.
		Log(LogLevel::Warning) << tag << " has no match for base faith: " << baseReligion;
		details.religion.clear();
	}
	// Change capitals for anyone not aztec.
	auto capitalMatchFound = false;
	if (tag != "AZT")
	{
		if (details.holder->getCharacterDomain() && details.holder->getCharacterDomain()->getRealmCapital().second)
		{
			const auto& realmCapital = details.holder->getCharacterDomain()->getRealmCapital().second;
			if (realmCapital->getDFLiege() && realmCapital->getDFLiege()->second)
			{
				const auto& capitalMatch = provinceMapper.getEU4ProvinceNumbers(realmCapital->getDFLiege()->second->getName());
				if (!capitalMatch.empty())
				{
					details.capital = *capitalMatch.begin();
					capitalMatchFound = true;
				}
			}
		}
		else
		{
			Log(LogLevel::Warning) << tag << " (" << title->first << ") holder has no character domain. Terrific.";
		}
	}

	if (!capitalMatchFound && tag != "AZT")
	{
		Log(LogLevel::Warning) << "No match for capital or missing capital for: " << tag;
		details.capital = 0;
	}

	// do we have a culture? Pope is special as always.
	std::string baseCulture;
	if (title->second->isThePope())
	{
		// This is getting absurd.
		baseCulture =
			 details.holder->getCharacterDomain()->getRealmCapital().second->getDJLiege()->second->getClay()->getCounty()->second->getCulture().second->getName();
	}
	else
	{
		if (details.holder->getCulture())
			baseCulture = details.holder->getCulture()->second->getName();
	}
	if (!baseCulture.empty())
	{
		const auto& cultureMatch = cultureMapper.cultureMatch(baseCulture, details.religion, details.capital, tag);
		if (cultureMatch)
			details.primaryCulture = *cultureMatch;
		else
		{
			// We failed to get a primaryCulture. This is not an issue. We'll set it later from the majority of owned provinces.
			Log(LogLevel::Warning) << "Failed to map ck3 culture: " << baseCulture << " into an EU4 culture. Check mappings!";
			details.primaryCulture.clear();
		}
	}
	else
	{
		Log(LogLevel::Warning) << tag << " has no ck3 culture! Will substitute with something.";
	}
	if (!details.primaryCulture.empty())
	{
		const auto& techMatch = cultureMapper.getTechGroup(details.primaryCulture);
		if (techMatch)
			details.technologyGroup = *techMatch;
		else
			Log(LogLevel::Warning) << details.primaryCulture << " has no tech group!  Check the mappings!";
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
	{
		details.holyRomanEmperor = true;
		Log(LogLevel::Info) << ">> HREmperor " << title->second->getName() << " imported into " << tag;
	}
	if (title->second->isInHRE())
		details.inHRE = true;
	// ditto for secondary_religion and harmonized religions.
	details.secondaryReligion.clear();
	details.harmonizedReligions.clear();
	details.historicalScore = 0; // Not sure about this.
}

void EU4::Country::populateCommons(const mappers::CultureMapper& cultureMapper, const mappers::LocalizationMapper& localizationMapper)
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
			auto actualName = previousHolder.second->getName();
			auto name = localizationMapper.getLocBlockForKey(previousHolder.second->getName());
			if (name)
				actualName = name->english;
			const auto& blockItr = details.monarchNames.find(actualName);
			if (blockItr != details.monarchNames.end())
				blockItr->second.first++;
			else
			{
				const auto female = previousHolder.second->isFemale();
				auto chance = 10;
				if (female)
					chance = -1;
				std::pair<int, int> newBlock = std::pair(1, chance);
				details.monarchNames.insert(std::pair(actualName, newBlock));
			}
		}
	}
	if (!title->second->getHolder()->second->getCourtierNames().empty())
	{
		for (const auto& courtier: title->second->getHolder()->second->getCourtierNames())
		{
			auto actualName = courtier.first;
			auto name = localizationMapper.getLocBlockForKey(actualName);
			if (name)
				actualName = name->english;
			const auto& blockItr = details.monarchNames.find(actualName);
			if (blockItr == details.monarchNames.end())
			{
				const auto female = !courtier.second;
				auto chance = 0;
				if (female)
					chance = -1;
				std::pair<int, int> newBlock = std::pair(0, chance);
				details.monarchNames.insert(std::pair(actualName, newBlock));
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

void EU4::Country::populateLocs(const mappers::LocalizationMapper& localizationMapper, bool dynasticNames)
{
	auto nameSet = false;

	// Pope is special, as always.
	if (title->second->isThePope() && tag == "PAP")
		nameSet = true; // We'll use vanilla PAP locs.

	// Displayname - custom renamed names take priority. They are only in english/player language.
	if (!nameSet && !title->second->getDisplayName().empty())
	{
		// This is a custom name in UTF8 encoding, usually english only. We copy it ad verbatum.
		mappers::LocBlock newblock;
		newblock.english = title->second->getDisplayName();
		newblock.spanish = title->second->getDisplayName();
		newblock.french = title->second->getDisplayName();
		newblock.german = title->second->getDisplayName();
		localizations.insert(std::pair(tag, newblock));
		nameSet = true;
	}

	// Then we see if we can match the locblocks for title. These are canonical names.

	if (!nameSet)
	{
		auto nameLocalizationMatch = localizationMapper.getLocBlockForKey(title->first);
		if (nameLocalizationMatch)
		{
			localizations.insert(std::pair(tag, *nameLocalizationMatch));
			nameSet = true;
		}
	}

	if (!nameSet)
	{
		// Now get creative. See if we can map any similar locs. Poke at it blindly.
		auto alternateName1 = "b_" + title->first.substr(2, title->first.length());
		auto alternateName2 = "c_" + title->first.substr(2, title->first.length());
		auto alternateName3 = "d_" + title->first.substr(2, title->first.length());
		auto alternateName4 = "e_" + title->first.substr(2, title->first.length());
		auto nameLocalizationMatch1 = localizationMapper.getLocBlockForKey(alternateName1);
		auto nameLocalizationMatch2 = localizationMapper.getLocBlockForKey(alternateName2);
		auto nameLocalizationMatch3 = localizationMapper.getLocBlockForKey(alternateName3);
		auto nameLocalizationMatch4 = localizationMapper.getLocBlockForKey(alternateName4);

		if (nameLocalizationMatch1)
		{
			localizations.insert(std::pair(tag, *nameLocalizationMatch1));
			nameSet = true;
		}
		if (!nameSet && nameLocalizationMatch2)
		{
			localizations.insert(std::pair(tag, *nameLocalizationMatch2));
			nameSet = true;
		}
		if (!nameSet && nameLocalizationMatch3)
		{
			localizations.insert(std::pair(tag, *nameLocalizationMatch3));
			nameSet = true;
		}
		if (!nameSet && nameLocalizationMatch4)
		{
			localizations.insert(std::pair(tag, *nameLocalizationMatch4));
			nameSet = true;
		}
	}
	if (!nameSet)
	{
		// using capital province name?
		auto capitalName = details.holder->getCharacterDomain()->getRealmCapital().second->getName();
		auto nameLocalizationMatch = localizationMapper.getLocBlockForKey(capitalName);
		if (nameLocalizationMatch)
		{
			localizations.insert(std::pair(tag, *nameLocalizationMatch));
			nameSet = true;
		}
	}

	// Override for kingdoms/empires that use Dynasty Names
	std::set<std::string> hardcodedExclusions =
		 {"k_rum", "k_israel", "e_india", "e_ilkhanate", "e_persia", "e_mali", "k_mali", "k_ghana", "k_songhay", "e_hre", "e_roman_empire", "e_byzantium"};

	if (dynasticNames && details.government == "monarchy" && !hardcodedExclusions.count(title->first) && details.holder->getHouse().first &&
		 details.holder->getHouse().second && details.holder->getHouse().second->getDynasty().second &&
		 details.holder->getHouse().second->getDynasty().second->isAppropriateRealmName() && title->second->getClay() &&
		 title->second->getClay()->canBeNamedAfterDynasty() &&
		 (title->second->getLevel() == CK3::LEVEL::KINGDOM || title->second->getLevel() == CK3::LEVEL::EMPIRE))
	{
		const auto& houseName = details.holder->getHouse().second->getName();
		auto nameLocalizationMatch = localizationMapper.getLocBlockForKey(houseName);
		if (nameLocalizationMatch)
		{
			mappers::LocBlock newblock;
			if (nameLocalizationMatch->english.back() == 's')
				newblock.english = nameLocalizationMatch->english;
			else
				newblock.english = nameLocalizationMatch->english + "s";
			newblock.spanish = nameLocalizationMatch->spanish;
			newblock.french = nameLocalizationMatch->french;
			newblock.german = nameLocalizationMatch->german;

			// If we already set a canonical name, don't just overwrite, save it for future reference (Ottoman Crimea)
			if (nameSet)
			{
				localizations.insert(std::pair("canonical", localizations[tag]));
			}

			localizations[tag] = newblock;
			details.hasDynastyName = true;
			nameSet = true;
		}
	}

	// giving up.
	if (!nameSet)
		Log(LogLevel::Warning) << tag << " needs help with localization! " << title->first;

	// --------------- Adjective Locs

	auto adjSet = false;

	// Pope is special, as always.
	if (title->second->isThePope())
	{
		adjSet = true; // We'll use vanilla PAP locs.
		return;
	}

	// For custom adjectives we use those directly.
	if (!adjSet && !title->second->getAdjective().empty())
	{
		// This is a custom adjective in UTF8 encoding, usually english only. We copy it ad verbatum.
		mappers::LocBlock newblock;
		newblock.english = title->second->getAdjective();
		newblock.spanish = title->second->getAdjective();
		newblock.french = title->second->getAdjective();
		newblock.german = title->second->getAdjective();
		localizations.insert(std::pair(tag + "_ADJ", newblock));
		adjSet = true;
	}

	// Except if needed override for dynasties. See if we use dynasty name.
	if (dynasticNames && details.government == "monarchy" && !hardcodedExclusions.count(title->first) && details.holder->getHouse().first &&
		 details.holder->getHouse().second->getDynasty().second->isAppropriateRealmName() &&
		 (title->second->getLevel() == CK3::LEVEL::KINGDOM || title->second->getLevel() == CK3::LEVEL::EMPIRE))
	{
		const auto& houseName = details.holder->getHouse().second->getName();
		auto nameLocalizationMatch = localizationMapper.getLocBlockForKey(houseName);
		if (nameLocalizationMatch)
		{
			mappers::LocBlock newblock;
			newblock.english = nameLocalizationMatch->english; // Ottoman Africa
			newblock.spanish = "de los " + nameLocalizationMatch->spanish;
			newblock.french = "des " + nameLocalizationMatch->french;
			newblock.german = nameLocalizationMatch->german + "-";
			if (localizations.contains(tag + "_ADJ"))
				localizations.at(tag + "_ADJ") = newblock;
			else
				localizations.emplace(tag + "_ADJ", newblock);
			adjSet = true;
		}
	}

	if (!adjSet)
	{
		// try with straight adj and cross fingers. Those usually work well.
		auto nameLocalizationMatch1 = localizationMapper.getLocBlockForKey(title->first + "_adj");
		auto nameLocalizationMatch2 = localizationMapper.getLocBlockForKey("b_" + title->first.substr(2, title->first.length()) + "_adj");
		auto nameLocalizationMatch3 = localizationMapper.getLocBlockForKey("c_" + title->first.substr(2, title->first.length()) + "_adj");
		auto nameLocalizationMatch4 = localizationMapper.getLocBlockForKey("d_" + title->first.substr(2, title->first.length()) + "_adj");
		auto nameLocalizationMatch5 = localizationMapper.getLocBlockForKey("k_" + title->first.substr(2, title->first.length()) + "_adj");
		if (nameLocalizationMatch1)
		{
			localizations.insert(std::pair(tag + "_ADJ", *nameLocalizationMatch1));
			adjSet = true;
		}
		if (!adjSet && nameLocalizationMatch2)
		{
			localizations.insert(std::pair(tag + "_ADJ", *nameLocalizationMatch2));
			adjSet = true;
		}
		if (!adjSet && nameLocalizationMatch3)
		{
			localizations.insert(std::pair(tag + "_ADJ", *nameLocalizationMatch3));
			adjSet = true;
		}
		if (!adjSet && nameLocalizationMatch4)
		{
			localizations.insert(std::pair(tag + "_ADJ", *nameLocalizationMatch4));
			adjSet = true;
		}
		if (!adjSet && nameLocalizationMatch5)
		{
			localizations.insert(std::pair(tag + "_ADJ", *nameLocalizationMatch5));
			adjSet = true;
		}
	}

	// Do we have a displayAdj?
	if (!adjSet && !title->second->getAdjective().empty())
	{
		// This is a custom name in UTF8 encoding, usually english only. We copy it ad verbatum.
		mappers::LocBlock newblock;
		newblock.english = title->second->getAdjective();
		newblock.spanish = title->second->getAdjective();
		newblock.french = title->second->getAdjective();
		newblock.german = title->second->getAdjective();
		localizations.insert(std::pair(tag + "_ADJ", newblock));
		adjSet = true;
	}

	// Can we use some custom name?
	if (!adjSet && !title->second->getDisplayName().empty())
	{
		mappers::LocBlock newblock;
		newblock.english = title->second->getDisplayName() + "'s"; // singular Nordarike's Africa
		newblock.spanish = "de " + title->second->getDisplayName();
		newblock.french = "de " + title->second->getDisplayName();
		newblock.german = title->second->getDisplayName() + "s";
		localizations.insert(std::pair(tag + "_ADJ", newblock));
		adjSet = true;
	}

	// out of ideas. Ignore everything and use name if possible.
	if (!adjSet)
	{
		Log(LogLevel::Warning) << tag << " needs help with localization for adjective! " << title->first << "_adj is missing. Using name substitute if possible.";
		if (nameSet && localizations.contains(tag))
			localizations.emplace(tag + "_ADJ", localizations.at(tag));
	}

	// Setting Up Idea Names
	if (adjSet && localizations.count(tag + "_ADJ") && !localizations.find(tag + "_ADJ")->second.english.empty())
	{
		mappers::LocBlock newblock;
		newblock.english = localizations.find(tag + "_ADJ")->second.english + " Ideas"; // Roman Ideas
		newblock.spanish = "Ideas de " + localizations.find(tag + "_ADJ")->second.spanish;
		newblock.french = "Doctrines " + localizations.find(tag + "_ADJ")->second.french;
		newblock.german = localizations.find(tag + "_ADJ")->second.german + " Ideen";
		localizations.insert(std::pair(tag + "_ideas", newblock));

		newblock.english = localizations.find(tag + "_ADJ")->second.english + " Traditions"; // Roman Traditions
		newblock.spanish = "Tradiciones de " + localizations.find(tag + "_ADJ")->second.spanish;
		newblock.french = "traditions " + localizations.find(tag + "_ADJ")->second.french;
		newblock.german = localizations.find(tag + "_ADJ")->second.german + " Traditionen";
		localizations.insert(std::pair(tag + "_ideas_start", newblock));

		newblock.english = localizations.find(tag + "_ADJ")->second.english + " Ambition"; // Roman Ambition
		newblock.spanish = "Ambici�n de " + localizations.find(tag + "_ADJ")->second.spanish;
		newblock.french = "ambitions " + localizations.find(tag + "_ADJ")->second.french;
		newblock.german = localizations.find(tag + "_ADJ")->second.german + " Ambitionen";
		localizations.insert(std::pair(tag + "_ideas_bonus", newblock));
	}
}

void EU4::Country::populateRulers(const mappers::ReligionMapper& religionMapper,
	 const mappers::CultureMapper& cultureMapper,
	 const mappers::RulerPersonalitiesMapper& rulerPersonalitiesMapper,
	 const mappers::LocalizationMapper& localizationMapper,
	 Configuration::STARTDATE startDateOption,
	 const date& theConversionDate,
	 const date& startDate)
{
	// do we HAVE a ruler? Broken saves come to mind.
	if (!details.holder || details.holder->isDead())
	{
		Log(LogLevel::Warning) << tag << " has no holder. Congratulations.";
		return;
	}

	// Does the supposed ruler have a domain at all? Hello broken saves.
	if (!details.holder->getCharacterDomain() || details.holder->getCharacterDomain()->getDomain().empty())
	{
		Log(LogLevel::Warning) << tag << "' holder has an empty domain. We can't work with this fellow, skipping ruler init.";
		return;
	}

	// Are we the ruler's primary title? (if he has any)
	// Potential PU's don't get monarchs. (and those apply for monarchies only)
	if (!details.holder->getCharacterDomain()->getDomain()[0].second)
		return; // corruption
	if (details.holder->getCharacterDomain()->getDomain()[0].second->getName() != title->first && details.government == "monarchy")
		return;

	convertHolder(rulerPersonalitiesMapper, localizationMapper, startDateOption, theConversionDate, startDate);
	convertSpouse(religionMapper, cultureMapper, rulerPersonalitiesMapper, localizationMapper, startDateOption, theConversionDate, startDate);
	convertHeirs(religionMapper, cultureMapper, rulerPersonalitiesMapper, localizationMapper, startDateOption, theConversionDate, startDate);

	// this transformation is always true, heir or not.
	if (conversionDate.diffInYears(details.monarch.birthDate) < 16)
	{
		details.heir = details.monarch;
		details.heir.monarchName.clear();
		details.heir.deathDate = details.heir.birthDate;
		details.heir.deathDate.subtractYears(-65);
		details.heir.claim = 89; // good enough?
		details.heir.adm = std::min(details.heir.adm + 2, 6);
		details.heir.mil = std::min(details.heir.mil + 2, 6);
		details.heir.dip = std::min(details.heir.dip + 2, 6);
		details.heir.personalities.clear();

		details.monarch.name = "(Regency Council)";
		details.monarch.regency = true;
		details.monarch.birthDate = date("1.1.1");
		details.monarch.female = false;
		details.monarch.dynasty.clear();
		details.monarch.personalities.clear();
	}
}

void EU4::Country::convertHeirs(const mappers::ReligionMapper& religionMapper,
	 const mappers::CultureMapper& cultureMapper,
	 const mappers::RulerPersonalitiesMapper& rulerPersonalitiesMapper,
	 const mappers::LocalizationMapper& localizationMapper,
	 Configuration::STARTDATE startDateOption,
	 const date& theConversionDate,
	 const date& startDate)
{
	if (!title->second->getHeirs().empty())
	{
		for (const auto& heir: title->second->getHeirs())
		{
			if (!heir.second || !heir.second->getHouse().second)
				continue;
			if (heir.second->isDead())
				continue; // This one is dead. Next, please.

			auto actualName = heir.second->getName();
			if (const auto& heirNameLoc = localizationMapper.getLocBlockForKey(actualName); heirNameLoc)
				actualName = heirNameLoc->english;
			details.heir.name = actualName;

			// We're setting future regnalness
			if (details.government != "republic" && !details.monarchNames.empty())
			{
				auto const& theName = heir.second->getName();
				std::string roman;
				if (details.monarchNames.contains(theName))
				{
					auto regnal = details.monarchNames.at(theName).first;
					if (regnal >= 1)
					{
						roman = " " + cardinalToRoman(regnal + 1);
					}
				}
				details.heir.monarchName = details.heir.name + roman;
			}

			std::string dynastyName;
			const auto& prefix = heir.second->getHouse().second->getPrefix();
			if (const auto& prefixLoc = localizationMapper.getLocBlockForKey(prefix); prefixLoc)
				dynastyName = prefixLoc->english;
			if (!heir.second->getHouse().second->getLocalizedName().empty())
			{
				dynastyName += heir.second->getHouse().second->getLocalizedName();
			}
			else
			{
				const auto& dynasty = heir.second->getHouse().second->getName();
				if (const auto& dynastyLoc = localizationMapper.getLocBlockForKey(dynasty); dynastyLoc)
					dynastyName += dynastyLoc->english;
				else
					dynastyName += dynasty; // There may be errors here with unresolved keys but it's not our fault.
			}
			details.heir.dynasty = dynastyName;

			details.heir.adm = std::min((heir.second->getSkills().stewardship + heir.second->getSkills().learning) / 2, 6);
			details.heir.dip = std::min((heir.second->getSkills().diplomacy + heir.second->getSkills().intrigue) / 2, 6);
			details.heir.mil = std::min((heir.second->getSkills().martial + heir.second->getSkills().learning) / 2, 6);
			details.heir.birthDate = normalizeDate(heir.second->getBirthDate(), startDateOption, theConversionDate, startDate);
			details.heir.female = heir.second->isFemale();
			if (!heir.second->getFaith() || !heir.second->getFaith()->second)
			{
				details.heir.religion = details.monarch.religion; // taking a shortcut.
			}
			else
			{
				if (const auto& religionMatch = religionMapper.getEU4ReligionForCK3Religion(heir.second->getFaith()->second->getName(),
						  heir.second->getFaith()->second->getReligiousHead());
					 religionMatch)
				{
					details.heir.religion = *religionMatch;
				}
				else
				{
					Log(LogLevel::Warning) << "No religion match for heir " << details.heir.name << ": " << heir.second->getFaith()->second->getName();
					details.heir.religion = details.monarch.religion; // taking a shortcut.
				}
			}
			if (!heir.second->getCulture() || !heir.second->getCulture()->second)
			{
				details.heir.culture = details.monarch.culture; // taking a shortcut.
			}
			else
			{
				if (const auto& cultureMatch = cultureMapper.cultureMatch(heir.second->getCulture()->second->getName(), details.heir.religion, 0, tag);
					 cultureMatch)
				{
					details.heir.culture = *cultureMatch;
				}
				else
				{
					Log(LogLevel::Warning) << "No culture match for heir " << details.heir.name << ": " << heir.second->getCulture()->second->getName();
					details.heir.culture = details.monarch.culture; // taking a shortcut.
				}
			}
			details.heir.deathDate = details.heir.birthDate;
			details.heir.deathDate.subtractYears(-65);
			details.heir.claim = 89; // good enough?
			details.heir.personalities = rulerPersonalitiesMapper.evaluatePersonalities(heir.second);
			details.heir.isSet = true;
			break;
		}
	}
}

void EU4::Country::convertSpouse(const mappers::ReligionMapper& religionMapper,
	 const mappers::CultureMapper& cultureMapper,
	 const mappers::RulerPersonalitiesMapper& rulerPersonalitiesMapper,
	 const mappers::LocalizationMapper& localizationMapper,
	 Configuration::STARTDATE startDateOption,
	 const date& theConversionDate,
	 const date& startDate)
{
	if (details.holder->getSpouse() && details.holder->getSpouse()->second && details.holder->getSpouse()->second->getHouse().second &&
		 !details.holder->isDead()) // making sure she's alive.
	{
		const auto spouse = details.holder->getSpouse()->second;

		auto actualName = spouse->getName();
		if (const auto& queenNameLoc = localizationMapper.getLocBlockForKey(actualName); queenNameLoc)
			actualName = queenNameLoc->english;
		details.queen.name = actualName;

		std::string dynastyName;
		const auto& prefix = spouse->getHouse().second->getPrefix();
		if (const auto& prefixLoc = localizationMapper.getLocBlockForKey(prefix); prefixLoc)
			dynastyName = prefixLoc->english;
		if (!spouse->getHouse().second->getLocalizedName().empty())
		{
			dynastyName += spouse->getHouse().second->getLocalizedName();
		}
		else
		{
			const auto& dynasty = spouse->getHouse().second->getName();
			if (const auto& dynastyLoc = localizationMapper.getLocBlockForKey(dynasty); dynastyLoc)
				dynastyName += dynastyLoc->english;
			else
				dynastyName += dynasty; // There may be errors here with unresolved keys but it's not our fault.
		}
		details.queen.dynasty = dynastyName;

		details.queen.adm = std::min((spouse->getSkills().stewardship + spouse->getSkills().learning) / 3, 6);
		details.queen.dip = std::min((spouse->getSkills().diplomacy + spouse->getSkills().intrigue) / 3, 6);
		details.queen.mil = std::min((spouse->getSkills().martial + spouse->getSkills().learning) / 3, 6);
		details.queen.birthDate = normalizeDate(spouse->getBirthDate(), startDateOption, theConversionDate, startDate);
		details.queen.female = spouse->isFemale();
		if (spouse->getFaith() && spouse->getFaith()->second)
		{
			if (const auto& religionMatch =
					  religionMapper.getEU4ReligionForCK3Religion(spouse->getFaith()->second->getName(), spouse->getFaith()->second->getReligiousHead());
				 religionMatch)
			{
				details.queen.religion = *religionMatch;
			}
			else
			{
				Log(LogLevel::Warning) << "No religion match for queen " << details.queen.name << ": " << spouse->getFaith()->second->getName();
				details.queen.religion = details.monarch.religion; // taking a shortcut.
			}
		}
		else
		{
			details.queen.religion = details.monarch.religion; // taking a shortcut.
		}
		if (spouse->getCulture() && spouse->getCulture()->second)
		{
			if (const auto& cultureMatch = cultureMapper.cultureMatch(spouse->getCulture()->second->getName(), details.queen.religion, 0, tag); cultureMatch)
			{
				details.queen.culture = *cultureMatch;
			}
			else
			{
				Log(LogLevel::Warning) << "No culture match for queen " << details.queen.name << ": " << spouse->getCulture()->second->getName();
				details.queen.culture = details.monarch.culture; // taking a shortcut.
			}
		}
		else
		{
			details.queen.culture = details.monarch.culture; // taking a shortcut.
		}
		details.queen.originCountry = tag;
		details.queen.deathDate = details.queen.birthDate;
		details.queen.deathDate.subtractYears(-60);
		details.queen.personalities = rulerPersonalitiesMapper.evaluatePersonalities(spouse);
		details.queen.isSet = true;
	}
}

void EU4::Country::convertHolder(const mappers::RulerPersonalitiesMapper& rulerPersonalitiesMapper,
	 const mappers::LocalizationMapper& localizationMapper,
	 Configuration::STARTDATE startDateOption,
	 const date& theConversionDate,
	 const date& startDate)
{
	// Determine regnalness.
	std::string actualName = details.holder->getName();
	if (const auto nameLoc = localizationMapper.getLocBlockForKey(actualName); nameLoc)
		actualName = nameLoc->english;
	if (details.government != "republic" && !details.monarchNames.empty())
	{
		std::string roman;
		const auto& nameItr = details.monarchNames.find(actualName);
		if (nameItr != details.monarchNames.end())
		{
			const int regnal = nameItr->second.first;
			if (regnal > 1)
			{
				roman = cardinalToRoman(regnal);
				roman = " " + roman;
			}
		}
		details.monarch.name = actualName + roman;
	}
	else
	{
		details.monarch.name = actualName;
	}

	if (details.holder->getHouse().second)
	{
		std::string dynastyName;
		const auto& prefix = details.holder->getHouse().second->getPrefix();
		const auto& prefixLoc = localizationMapper.getLocBlockForKey(prefix);
		if (prefixLoc)
			dynastyName = prefixLoc->english;
		if (!details.holder->getHouse().second->getLocalizedName().empty())
		{
			dynastyName += details.holder->getHouse().second->getLocalizedName();
		}
		else
		{
			const auto& dynasty = details.holder->getHouse().second->getName();
			const auto& dynastyLoc = localizationMapper.getLocBlockForKey(dynasty);
			if (dynastyLoc)
				dynastyName += dynastyLoc->english;
			else
				dynastyName += dynasty; // There may be errors here with unresolved keys but it's not our fault.
		}
		details.monarch.dynasty = dynastyName;
	}

	details.monarch.adm = std::min((details.holder->getSkills().stewardship + details.holder->getSkills().learning) / 3, 6);
	details.monarch.dip = std::min((details.holder->getSkills().diplomacy + details.holder->getSkills().intrigue) / 3, 6);
	details.monarch.mil = std::min((details.holder->getSkills().martial + details.holder->getSkills().learning) / 3, 6);
	details.monarch.birthDate = normalizeDate(details.holder->getBirthDate(), startDateOption, theConversionDate, startDate);
	details.monarch.female = details.holder->isFemale();
	// religion and culture were already determining our country's primary culture and religion. If we set there, we'll copy here.
	if (!details.primaryCulture.empty())
		details.monarch.culture = details.primaryCulture;
	if (!details.religion.empty())
		details.monarch.religion = details.religion;
	details.monarch.personalities = rulerPersonalitiesMapper.evaluatePersonalities(details.holder);
	details.monarch.isSet = true;
}

int EU4::Country::getDevelopment() const
{
	auto dev = 0;
	for (const auto& province: provinces)
		dev += province.second->getDev();
	return dev;
}

bool EU4::Country::verifyCapital(const mappers::ProvinceMapper& provinceMapper)
{
	// We have set a provisionary capital earlier, but now we can check if it's valid.
	if (!title)
		return false;
	if (provinces.empty())
		return false;
	if (details.capital && provinces.count(details.capital))
		return false;

	// Embrace the brokenness.
	if (!details.holder->getCharacterDomain() || !details.holder->getCharacterDomain()->getRealmCapital().second ||
		 !details.holder->getCharacterDomain()->getRealmCapital().second->getDFLiege() ||
		 !details.holder->getCharacterDomain()->getRealmCapital().second->getDFLiege()->second)
	{
		Log(LogLevel::Warning) << tag << " has likely a broken capital due to save corruption. Remapping.";
		details.capital = provinces.begin()->second->getProvinceID();
		return true;
	}

	const auto& holderCapitalCounty = details.holder->getCharacterDomain()->getRealmCapital().second->getDFLiege()->second;
	const auto& capitalMatch = provinceMapper.getEU4ProvinceNumbers(holderCapitalCounty->getName());
	if (!capitalMatch.empty())
	{
		for (const auto& provinceID: capitalMatch)
		{
			if (provinces.count(provinceID))
			{
				details.capital = provinceID;
				return true;
			}
		}
	}
	// Use any other province.
	details.capital = provinces.begin()->second->getProvinceID();
	return true;
}

void EU4::Country::setPrimaryCulture(const std::string& culture)
{
	details.primaryCulture = culture;
	if (details.monarch.isSet && details.monarch.culture.empty())
		details.monarch.culture = culture;
	if (details.queen.isSet && details.queen.culture.empty())
		details.queen.culture = culture;
	if (details.heir.isSet && details.heir.culture.empty())
		details.heir.culture = culture;
}

// All cultures that make up more than 33% of a countries dev will start the game as accepted
void EU4::Country::setAcceptedCultures()
{
	std::map<std::string, int> cultureDevelopment; // culture, development
	int substantialDev = 0;
	for (const auto& province: provinces)
	{
		substantialDev += province.second->getDev();
		if (!province.second->getCulture().empty())
			cultureDevelopment[province.second->getCulture()] += province.second->getDev();
	}
	substantialDev /= 3;
	for (const auto& culture: cultureDevelopment)
	{
		if (culture.second >= substantialDev && culture.first != details.primaryCulture)
			details.acceptedCultures.insert(culture.first);
	}
}

void EU4::Country::setMajorityReligion(const std::string& religion)
{
	details.majorityReligion = religion;
}

void EU4::Country::setReligion(const std::string& religion)
{
	details.religion = religion;
	if (details.monarch.isSet && details.monarch.religion.empty())
		details.monarch.religion = religion;
	if (details.queen.isSet && details.queen.religion.empty())
		details.queen.religion = religion;
	if (details.heir.isSet && details.heir.religion.empty())
		details.heir.religion = religion;
}

void EU4::Country::assignReforms(const std::shared_ptr<mappers::RegionMapper>& regionMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const mappers::CultureDefinitionsMapper& cultureDefinitionMapper)
{
	// TODO; This entire function needs love. I'm not dropping the code but many aspects won't work.

	// Setting the Primary Religion (The religion most common in the country, not the religion of the country, needed for some reforms)
	if (details.majorityReligion.empty())
	{
		std::map<std::string, int> religionDevelopment; // religion, development
		int primeDev = -1;
		std::string primeReligion;
		for (const auto& province: provinces)
		{
			religionDevelopment[province.second->getReligion()] += province.second->getDev();
			if (religionDevelopment[province.second->getReligion()] > primeDev)
			{
				primeReligion = province.second->getReligion();
				primeDev = religionDevelopment[province.second->getReligion()];
			}
		}
		setMajorityReligion(primeReligion);
	}

	// Sets Accepted Cultures
	setAcceptedCultures();

	// Checking to see if we have a center of trade with level 2 or higher
	bool hasTradeCenterLevelTwo = false;
	for (const auto& province: provinces)
	{
		if (province.second->getCenterOfTradeLevel() >= 2)
		{
			hasTradeCenterLevelTwo = true;
			break;
		}
	}

	// RELIGIONS
	// Muslims
	std::set<std::string> muslimReligions = {"sunni", "zikri", "yazidi", "ibadi", "kharijite", "shiite", "druze", "hurufi", "qarmatian"};
	for (const auto& religion: religionMapper.getGeneratedReligions())
		if (religion.religionGroup == "muslim")
			muslimReligions.insert(religion.name);
	// Mazdans
	std::set<std::string> mazdanReligions = {"zoroastrian", "zurvanism", "gayomarthianism", "mazdaki", "manichean", "khurmazta", "khurramism", "urartuism"};
	for (const auto& religion: religionMapper.getGeneratedReligions())
		if (religion.religionGroup == "zoroastrian_group")
			mazdanReligions.insert(religion.name);
	// Buddhists - TODO(#41): Re-enable when we can map to these religions
	// std::set<std::string> buddhistReligions = { "buddhism", "vajrayana", "mahayana" };
	// Eastern
	std::set<std::string> easternReligions = {"confucianism", "shinto", "buddhism", "vajrayana", "mahayana"};
	for (const auto& religion: religionMapper.getGeneratedReligions())
		if (religion.religionGroup == "eastern")
			easternReligions.insert(religion.name);
	// Indians (Dharmic + Buddhists)
	std::set<std::string> indianReligions = {"buddhism", "vajrayana", "mahayana", "hinduism", "jain"};
	for (const auto& religion: religionMapper.getGeneratedReligions())
		if (religion.religionGroup == "dharmic" || religion.parent == "buddhism" || religion.parent == "vajrayana" || religion.parent == "mahayana")
			indianReligions.insert(religion.name);
	// Protestants
	std::set<std::string> protestantReligions = {"protestant", "reformed", "hussite", "cathar", "waldensian", "lollard", "adamites"};
	for (const auto& religion: religionMapper.getGeneratedReligions())
		if (religion.religionGroup == "christian" &&
			 (religion.country.find("uses_church_power") != std::string::npos || religion.country.find("fervor") != std::string::npos ||
				  religion.country.find("uses_anglican_power") != std::string::npos || religion.country.find("uses_hussite_power") != std::string::npos))
			protestantReligions.insert(religion.name);
	// Orthodox
	std::set<std::string> orthodoxReligions = {"orthodox", "monothelite", "iconoclast", "paulician", "bogomilist"};
	for (const auto& religion: religionMapper.getGeneratedReligions())
		if (religion.religionGroup == "christian" && religion.country.find("has_patriarchs") != std::string::npos)
			orthodoxReligions.insert(religion.name);
	// Pagan
	std::set<std::string> paganReligions = {"pagan_religion",
		 "pagan_religion_reformed",
		 "aztec_pagan",
		 "baltic_pagan",
		 "baltic_pagan_reformed",
		 "finnish_pagan",
		 "finnish_pagan_reformed"
		 "norse_pagan",
		 "norse_pagan_reformed",
		 "slavic_pagan",
		 "slavic_pagan_reformed",
		 "tengri_pagan",
		 "tengri_pagan_reformed",
		 "magyar_pagan",
		 "magyar_pagan_reformed",
		 "siberian_pagan",
		 "siberian_pagan_reformed",
		 "zun_pagan",
		 "zun_pagan_reformed",
		 "bon",
		 "bon_reformed",
		 "hellenic_pagan",
		 "hellenic_pagan_reformed",
		 "west_african_pagan",
		 "west_african_pagan_reformed",
		 "shamanism",
		 "animism",
		 "totemism",
		 "inti",
		 "nahuatl",
		 "mesoamerican_religion",
		 "kushitism_pagan",
		 "kushitism_reformed",
		 "germanic_pagan",
		 "anglo_pagan",
		 "scythian_pagan",
		 "zamolxism",
		 "mithraism",
		 "iberian_pagan",
		 "canaanite",
		 "kemetism",
		 "mardukism",
		 "arab_pagan",
		 "cybelene",
		 "armenian_pagan",
		 "caucasian_pagan",
		 "atenism",
		 "druidism",
		 "briton_pagan",
		 "gallic_pagan",
		 "gaelic_pagan",
		 "celtiberian_pagan",
		 "harranism"};
	for (const auto& religion: religionMapper.getGeneratedReligions())
		if (religion.religionGroup == "pagan")
			paganReligions.insert(religion.name);
	for (const auto& religion: religionMapper.getReformedReligions())
		paganReligions.insert(religion);

	// CULTURES
	// Baltic Culture Group
	std::set<std::string> balticCultures = {"estonian", "lithuanian", "latvian", "old_prussian"};
	for (const auto& customCultures: cultureDefinitionMapper.getCultureGroupsMap().find("baltic")->second->getCultures())
		balticCultures.insert(customCultures.first);
	// Chinese Culture Group
	std::set<std::string> chineseCultures = {"vietnamese_new",
		 "korean_new",
		 "tibetan_new",
		 "altaic_new",
		 "manchu_new",
		 "chihan",
		 "cantonese",
		 "jin",
		 "wu",
		 "chimin",
		 "hakka",
		 "gan",
		 "xiang",
		 "sichuanese",
		 "jianghuai",
		 "xibei",
		 "hubei",
		 "zhongyuan",
		 "shandong_culture"};
	for (const auto& customCultures: cultureDefinitionMapper.getCultureGroupsMap().find("east_asian")->second->getCultures())
		chineseCultures.insert(customCultures.first);
	// Dravidian Culture Group
	std::set<std::string> dravidianCultures = {"tamil", "telegu", "kannada", "malayalam"};
	for (const auto& customCultures: cultureDefinitionMapper.getCultureGroupsMap().find("dravidian")->second->getCultures())
		dravidianCultures.insert(customCultures.first);
	// Mande Culture Group
	std::set<std::string> mandeCultures = {"mali", "songhai", "soninke", "bambara", "bozo", "dyola"};
	for (const auto& customCultures: cultureDefinitionMapper.getCultureGroupsMap().find("mande")->second->getCultures())
		mandeCultures.insert(customCultures.first);
	// Roman Culture Group
	std::set<std::string> latinCultures = {"lombard",
		 "tuscan",
		 "sardinian",
		 "romagnan",
		 "ligurian",
		 "venetian",
		 "dalmatian ",
		 "neapolitan",
		 "piedmontese",
		 "umbrian",
		 "sicilian",
		 "maltese",
		 "italian"};
	for (const auto& customCultures: cultureDefinitionMapper.getCultureGroupsMap().find("latin")->second->getCultures())
		latinCultures.insert(customCultures.first);
	// Russian Cultures (Not all East Slavic)
	std::set<std::string> russianCultures =
		 {"russian", "ilmenian", "severian", "volhynian", "russian_culture", "novgorodian", "ryazanian", "byelorussian", "ruthenian"};
	for (const auto& customCultures:
		 cultureDefinitionMapper.getCultureGroupsMap().find("east_slavic")->second->getCultures()) // Benefit of the doubt for dynamic cultures
		russianCultures.insert(customCultures.first);
	// Western Aryan Culture Group
	std::set<std::string> westAryanCultures = {"gujarati", "saurashtri", "marathi", "sindhi", "rajput", "malvi"};
	for (const auto& customCultures: cultureDefinitionMapper.getCultureGroupsMap().find("western_aryan")->second->getCultures())
		westAryanCultures.insert(customCultures.first);

	// OTHER
	std::set<std::string> partitionSuccession = {"confederate_partition_succession_law", "partition_succession_law", "high_partition_succession_law"};
	std::set<std::string> electiveMonarchies = {"feudal_elective_succession_law", "saxon_elective_succession_law", "scandinavian_elective_succession_law"};
	//*Note* - Tanistry is excluded for being Dynasty specific, and Princely is excluded for being a special snowflake law.

	// GENERIC REFORMS - TODO(#41): Re-enable when we can map those laws to actual reforms. This code is valid for CK3.

	std::set<std::string> laws = title->second->getLaws();

	// Sanity check before proceeding.
	if (!details.holder->getCharacterDomain())
		return; // break and forget about reforms for this one.

	std::string governmentType = "despotic";
	if (details.holder->getCharacterDomain()->getLaws().count("tribal_authority_3") ||
		 details.holder->getCharacterDomain()->getLaws().count("crown_authority_3"))
		governmentType = "absolute";
	else if (details.holder->getCharacterDomain()->getLaws().count("tribal_authority_0") ||
				details.holder->getCharacterDomain()->getLaws().count("crown_authority_0"))
		governmentType = "aristocratic";

	// MONARCHIES
	if (details.government == "monarchy")
	{
		// Iqta
		if (muslimReligions.count(details.religion) && governmentType == "aristocratic")
		{
			details.reforms.clear();
			details.reforms = {"iqta"};
		}
		// Feudal Theocracy
		if ((muslimReligions.count(details.religion) && governmentType == "despotic") ||
			 (mazdanReligions.count(details.religion) && governmentType != "aristocratic"))
		{
			details.reforms.clear();
			details.reforms = {"feudal_theocracy"};
		}
		// Feudalism
		else if (governmentType == "despotic")
		{
			details.reforms.clear();
			details.reforms = {"feudalism_reform"};
		}
		// English Monarchy (Renamed in the converter)
		else if (governmentType == "aristocratic")
		{
			details.reforms.clear();
			details.reforms = {"english_monarchy"};
		}
		// Berylik government. Ottoman Government (Renamed in converter) is currently disabled due to decadence being tied to TUR alone.
		else if (muslimReligions.count(details.religion) && governmentType == "absolute")
		{
			details.reforms.clear();
			details.reforms = {"beylik_government"};
		}
		// Autocracy, also the fallback
		else // if (governmentType == "absolute")
		{
			details.reforms.clear();
			details.reforms = {"autocracy_reform"};
		}
		for (const auto& law: details.holder->getCharacterDomain()->getLaws())
		{
			// Electoral
			if (electiveMonarchies.count(law) && tag != "ROM" && tag != "HLR" && tag != "BYZ")
			{
				details.reforms.clear();
				details.reforms = {"elective_monarchy"};
				break;
			}
		}
	}

	// REPUBLICS
	if (details.government == "republic")
	{
		// No republic laws for now. Waiting for a DLC to bring some.
		// Mercs
		if (details.holder->getCharacterDomain()->getLaws().count("mercenary_company_succession_law"))
		{
			details.reforms.clear();
			details.reforms = {"noble_elite_reform"};
		}
	}

	// TRIBES
	// Just setting some locational ones. Not enough laws to do proper differentiation.
	if (details.government == "tribal")
	{
		bool isReformed = false;
		if (paganReligions.contains(details.religion))
			for (const auto& religion: religionMapper.getReformedReligions())
				if (religion == details.religion)
				{
					isReformed = true;
					break;
				}
		/*
		 * This is now done through govmap.txt for these two.
		 *
		 // Hordes (This is the worst way to do this, but so be it until we get a horde government type)
		 if (regionMapper->provinceIsInRegion(details.capital, "tambov_area") || regionMapper->provinceIsInRegion(details.capital, "ryazan_area") ||
			  regionMapper->provinceIsInRegion(details.capital, "suzdal_area") || regionMapper->provinceIsInRegion(details.capital, "sloboda_ukraine_area") ||
			  regionMapper->provinceIsInRegion(details.capital, "ural_region") || regionMapper->provinceIsInRegion(details.capital, "crimea_region") ||
			  regionMapper->provinceIsInRegion(details.capital, "central_asia_region") || regionMapper->provinceIsInRegion(details.capital, "mongolia_region"))
		 {
			 details.reforms.clear();
			 details.reforms = {"steppe_horde"};
		 }
		 // Siberian Tribes
		 else if (regionMapper->provinceIsInRegion(details.capital, "west_siberia_region") ||
					 regionMapper->provinceIsInRegion(details.capital, "east_siberia_region"))
		 {
			 details.reforms.clear();
			 details.reforms = {"siberian_tribe"};
		 }
		 */
		if (details.reforms.contains("steppe_horde") || details.reforms.contains("siberian_tribe"))
			return;

		// Tribal Federations
		if (muslimReligions.count(details.religion))
		{
			details.reforms.clear();
			details.reforms = {"tribal_federation"};
		}
		// Sacred Kingdom
		else if (paganReligions.contains(details.religion) && governmentType == "absolute" &&
					religionMapper.getCK3ReligiousDoctrines().contains("doctrine_temporal_head"))
		{
			details.reforms.clear();
			details.reforms = {"sacred_kingdom"};
		}
		// Matrilineal System
		else if (details.holder->getCharacterDomain()->getLaws().contains("equal_law") ||
					details.holder->getCharacterDomain()->getLaws().contains("female_preference_law") ||
					details.holder->getCharacterDomain()->getLaws().contains("female_only_law"))
		{
			details.reforms.clear();
			details.reforms = {"matrilineal_system"};
		}
		else
		{
			bool wasKing = false;
			for (const auto& law: details.holder->getCharacterDomain()->getLaws())
				if (partitionSuccession.contains(law))
				{
					// Feudal Tribe
					if (details.holder->getCharacterDomain()->getLaws().contains("tribal_authority_3"))
					{
						details.reforms.clear();
						details.reforms = {"feudal_tribe"};
					}
					// Great Man
					else if (details.holder->getCharacterDomain()->getLaws().contains("tribal_authority_2"))
					{
						details.reforms.clear();
						details.reforms = {"great_man"};
					}
					// Tribal Kingdoms
					else if (details.holder->getCharacterDomain()->getLaws().contains("tribal_authority_1"))
					{
						details.reforms.clear();
						details.reforms = {"tribal_kingdom"};
					}
					// Tribal Despotism
					else if (details.holder->getCharacterDomain()->getLaws().contains("tribal_authority_0"))
					{
						details.reforms.clear();
						details.reforms = {"tribal_despotism"};
					}
					wasKing = true;
					break;
				}

			if (!wasKing)
			{
				// Stateless Society
				if (provinces.size() == 1 && paganReligions.contains(details.religion) && !isReformed)
				{
					details.reforms.clear();
					details.reforms = {"stateless_society"};
				}
				// Tribal Confederacy, also the fallback
				else
				{
					details.reforms.clear();
					details.reforms = {"tribal_confederacy"};
				}
			}
		}
	}
	// THEOCRACIES
	if (details.government == "theocracy")
	{
		// Papacy
		if (tag == "PAP" || title->first == "k_papal_state")
		{
			details.reforms.clear();
			details.reforms = {"papacy_reform"};
		}
		// Prussian Theocracy (Not named as such in loc)
		else if (tag == "PRU" && protestantReligions.count(details.religion))
		{
			details.reforms.clear();
			details.reforms = {"prussian_theocratic_reform"};
		}
		// Holy Horde
		else if ((details.religion == "catholic") && details.holder->getCharacterDomain()->getLaws().count("holy_order_succession_law") &&
					details.holder->getCharacterDomain()->getLaws().count("holy_order_succession_law") &&
					(regionMapper->provinceIsInRegion(details.capital, "tambov_area") || regionMapper->provinceIsInRegion(details.capital, "ryazan_area") ||
						 regionMapper->provinceIsInRegion(details.capital, "suzdal_area") ||
						 regionMapper->provinceIsInRegion(details.capital, "sloboda_ukraine_area") ||
						 regionMapper->provinceIsInRegion(details.capital, "ural_region") || regionMapper->provinceIsInRegion(details.capital, "crimea_region") ||
						 regionMapper->provinceIsInRegion(details.capital, "central_asia_region") ||
						 regionMapper->provinceIsInRegion(details.capital, "mongolia_region")))
		{
			details.reforms.clear();
			details.reforms = {"holy_horde_reform"};
		}
		// Holy Orders
		else if (details.holder->getCharacterDomain()->getLaws().count("holy_order_succession_law"))
		{
			details.reforms.clear();
			details.reforms = {"monastic_order_reform"};
		}
		// Crusader States
		else if ((details.religion == "catholic") && (tag == "KOJ" || tag == "EGY"))
		{
			details.reforms.clear();
			details.reforms = {"holy_state_reform"};
		}
		// All Other Theocracies
		else
		{
			details.reforms.clear();
			details.reforms = {"leading_clergy_reform"};
		}
	}

	// SPECIFIC REFORMS
	// Great Mongolia (Mongol Empire)
	if (tag == "MGE" && details.government == "tribal")
	{
		details.reforms.clear();
		details.reforms = {"great_mongol_state_reform"};
	}
	// Shaka Regime (Renamed in converter)
	else if ((details.primaryCulture == "prussian" || details.primaryCulture == "old_prussian") && details.government == "tribal" &&
				!details.reforms.contains("tribal_confederacy"))
	{
		details.reforms.clear();
		details.reforms = {"shaka_regime"};
	}
	// Negusa Nagast
	else if (tag == "ETH" && details.government == "feudal" && !(details.governmentRank == 3 && governmentType == "absolute"))
	{
		details.reforms.clear();
		details.reforms = {"negusa_nagast"};
	}
	// Solomonic Empire
	else if (tag == "ETH" && details.government == "feudal" && details.governmentRank == 3 && governmentType == "absolute")
	{
		details.reforms.clear();
		details.reforms = {"solomonic_empire"};
	}
	// Mughal Diwan System is a LEVEL 2 REFORM

	// TODO(#41): Most of the stuff below needs a rework as it depends on stuff like english monarchy or iqta which we cannot set at this stage.
	// TODO(#41): The code is not broken, so I'm not removing it.

	// Austrian Archduchy (Renamed in converter)
	else if (details.government == "monarchy" && (tag == "HAB" || isHREEmperor()))
	{
		details.reforms.clear();
		details.reforms = {"austrian_archduchy_reform"};
	}
	// Prussian Monarchy (Renamed in converter)
	else if (details.government == "monarchy" && tag == "PRU" && protestantReligions.count(details.religion))
	{
		details.reforms.clear();
		details.reforms = {"prussian_monarchy"};
	}
	// Tsardom
	else if (details.government == "monarchy" &&
				(tag == "UKR" || (tag == "RUS" && (orthodoxReligions.count(details.religion) || paganReligions.count(details.religion)))))
	{
		details.reforms.clear();
		details.reforms = {"tsardom"};
	}
	// Principality
	else if (details.government == "monarchy" && details.governmentRank != 3 &&
				(orthodoxReligions.count(details.religion) || details.religion == "slavic_pagan" || details.religion == "slavic_pagan_reformed") &&
				russianCultures.count(details.primaryCulture) && tag != "POL" && tag != "PAP" && tag != "HLR")
	{
		details.reforms.clear();
		details.reforms = {"principality"};
	}
	// Mamluk (Renamed in converter)
	else if (muslimReligions.count(details.religion) && title->second->getLaws().count("mercenary_company_succession_law") &&
				(regionMapper->provinceIsInRegion(details.capital, "near_east_superregion") ||
					 regionMapper->provinceIsInRegion(details.capital, "eastern_europe_superregion") ||
					 regionMapper->provinceIsInRegion(details.capital, "persia_superregion") || regionMapper->provinceIsInRegion(details.capital, "egypt_region") ||
					 regionMapper->provinceIsInRegion(details.capital, "maghreb_region") ||
					 regionMapper->provinceIsInRegion(details.capital, "central_asia_region")))
	{
		if (!details.acceptedCultures.count("circassian") && details.primaryCulture != "circassian")
			details.acceptedCultures.insert("circassian");
		details.government.clear();
		details.government = "monarchy";
		details.reforms.clear();
		details.reforms = {"mamluk_government"};
	}
	// Indian Sultanate (Renamed in Converter)
	else if ((details.reforms.count("feudalism_reform") || details.reforms.count("autocracy_reform") || details.reforms.count("iqta") ||
					 details.reforms.count("ottoman_government")) &&
				muslimReligions.count(details.religion) && indianReligions.count(details.majorityReligion))
	{
		details.reforms.clear();
		details.reforms = {"indian_sultanate_reform"};
	}
	// Mandala
	else if ((details.reforms.count("feudalism_reform") || details.reforms.count("english_monarchy")) && details.technologyGroup == "chinese" &&
				(paganReligions.count(details.religion) || muslimReligions.count(details.religion) || easternReligions.count(details.religion) ||
					 indianReligions.count(details.religion)))
	{
		details.reforms.clear();
		details.reforms = {"mandala_reform"};
	}
	// Nayankara
	else if ((details.reforms.count("feudalism_reform") || details.reforms.count("english_monarchy")) && indianReligions.count(details.religion) &&
				details.technologyGroup == "indian" &&
				(dravidianCultures.count(details.primaryCulture) || details.primaryCulture == "oriya" || details.primaryCulture == "sinhala"))
	{
		details.reforms.clear();
		details.reforms = {"nayankara_reform"};
	}
	// Rajput
	else if ((details.reforms.count("feudalism_reform") || details.reforms.count("english_monarchy") || details.reforms.count("autocracy_reform")) &&
				details.technologyGroup == "indian" && details.primaryCulture != "marathi" &&
				(details.primaryCulture == "vindhyan" || westAryanCultures.count(details.primaryCulture)))
	{
		details.reforms.clear();
		details.reforms = {"rajput_kingdom"};
	}
	// Gond Kingdom
	else if ((details.reforms.count("feudalism_reform") || details.reforms.count("english_monarchy") || details.reforms.count("autocracy_reform")) &&
				details.technologyGroup == "indian" && details.primaryCulture == "gondi")
	{
		details.reforms.clear();
		details.reforms = {"gond_kingdom"};
	}
	// Upraja
	else if ((details.reforms.count("feudalism_reform") || details.reforms.count("english_monarchy") || details.reforms.count("autocracy_reform")) &&
				easternReligions.count(details.religion) && details.religion != "confucianism" && details.religion != "shinto")
	{
		details.reforms.clear();
		details.reforms = {"uparaja_reform"};
	}
	// Mossi Federal Kingdom
	else if ((details.reforms.count("feudalism_reform") || details.reforms.count("english_monarchy")) && (details.primaryCulture == "mossi" || tag == "MSI"))
	{
		details.reforms.clear();
		details.reforms = {"mossi_federal_kingdom"};
	}
	// Musa Rule
	else if (details.reforms.count("autocracy") && (mandeCultures.contains(details.primaryCulture) || tag == "MLI"))
	{
		details.reforms.clear();
		details.reforms = {"musa_rule"};
	}
	// Plutocratic
	else if ((details.reforms.count("feudalism_reform") || details.reforms.count("english_monarchy") ||
					 ((details.reforms.count("autocracy_reform") || details.reforms.count("ottoman_government")) && details.governmentRank != 3)) &&
				(details.technologyGroup == "indian" || details.technologyGroup == "muslim" || details.technologyGroup == "chinese" ||
					 details.technologyGroup == "east_african") &&
				hasTradeCenterLevelTwo)
	{
		details.reforms.clear();
		details.reforms = {"plutocratic_reform"};
		details.mercantilism = 15;
	}
	// Grand Duchy
	else if ((details.reforms.count("feudalism_reform") || details.reforms.count("english_monarchy") || details.reforms.count("autocracy_reform")) &&
				details.governmentRank == 1 &&
				(tag == "LUX" || tag == "BAD" || tag == "TUS" || tag == "FIN" || tag == "LIT" || details.primaryCulture == "finnish" ||
					 balticCultures.count(details.primaryCulture)))
	{
		details.reforms.clear();
		details.reforms = {"grand_duchy_reform"};
	}
	// Chinese Kingdom
	else if (details.reforms.count("autocracy_reform") && chineseCultures.count(details.primaryCulture))
	{
		details.reforms.clear();
		details.reforms = {"chinese_warlord"};
	}

	// Peasant Republic
	else if (details.government != "theocracy" && tag != "ROM" && tag != "HLR" && tag != "BYZ" && details.holder->hasTrait("peasant_leader") &&
				(provinces.size() == 1 || details.government == "republic"))
	{
		details.government.clear();
		details.government = "republic";
		details.reforms.clear();
		details.reforms = {"peasants_republic"};
	}
	else if (details.reforms.count("peasants_republic") && details.primaryCulture == "swiss")
	{
		details.reforms.clear();
		details.reforms = {"united_cantons_reform"};
	}
	// Free City (HRE) - These have already been set in EU4World.cpp by the setFreeCities() method

	// Veche Republic
	else if (details.government == "republic" && details.governmentRank != 3 &&
				(orthodoxReligions.count(details.religion) || paganReligions.contains(details.religion)) && russianCultures.count(details.primaryCulture) &&
				tag != "POL" && tag != "PAP" && tag != "HLR")
	{
		details.reforms.clear();
		details.reforms = {"veche_republic"};
		details.mercantilism = 25;
	}
	// Prussian Republic (Renamed in converter)
	else if (details.government == "republic" && tag == "PRU" && protestantReligions.count(details.religion))
	{
		details.reforms.clear();
		details.reforms = {"prussian_republic_reform"};
	}
	// Signoria
	else if (details.government == "republic" && !details.reforms.count("merchants_reform") && !details.reforms.count("venice_merchants_reform") &&
				!details.reforms.count("free_city") && latinCultures.count(details.primaryCulture))
	{
		details.reforms.clear();
		details.reforms = {"signoria_reform"};
		details.mercantilism = 15;
	}
	// Trading City
	else if (details.reforms.count("merchants_reform") && provinces.size() == 1)
	{
		details.reforms.clear();
		details.reforms = {"trading_city"};
		details.mercantilism = 25;
	}
	// Venetian Republic
	else if (details.government == "republic" && tag == "VEN")
	{
		details.reforms.clear();
		details.reforms = {"venice_merchants_reform"};
		details.mercantilism = 25;
	}
	// Dutch Republic
	else if (details.government == "republic" && tag == "NED")
	{
		details.reforms.clear();
		details.reforms = {"dutch_republic"};
		details.mercantilism = 15;
	}
}

void EU4::Country::initializeAdvisers(const mappers::LocalizationMapper& localizationMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const mappers::CultureMapper& cultureMapper)
{
	// We're doing this one separate from initial country generation so that country's primary culture and religion may have had time to get
	// initialized.
	if (!title || !details.holder)
		return; // Vanilla and the dead do not get these.
	// Sanity check
	if (!details.holder->getCharacterDomain() || details.holder->getCharacterDomain()->getDomain().empty())
		return; // hello broken.
	if (!details.holder->getCharacterDomain()->getDomain()[0].second)
		return; // hello double broken
	if (details.holder->getCharacterDomain()->getDomain()[0].second->getName() != title->first)
		return; // PU's don't get advisors on secondary countries.

	for (const auto& adviser: details.holder->getCouncilors())
	{
		if (adviser.second->isSpent())
			continue;
		Character newAdviser;
		newAdviser.name = adviser.second->getName();
		auto localizedName = localizationMapper.getLocBlockForKey(newAdviser.name);
		if (localizedName)
			newAdviser.name = localizedName->english;
		if (adviser.second->getHouse().first)
		{
			newAdviser.prefix = adviser.second->getHouse().second->getPrefix();
			auto localizedPrefix = localizationMapper.getLocBlockForKey(newAdviser.prefix);
			if (localizedPrefix)
				newAdviser.prefix = localizedPrefix->english;
			newAdviser.surname = adviser.second->getHouse().second->getName();
			auto localizedSurname = localizationMapper.getLocBlockForKey(newAdviser.surname);
			if (localizedSurname)
				newAdviser.surname = localizedSurname->english;
		}
		if (details.capital)
			newAdviser.location = details.capital;
		// In CK3 advisors don't have jobs but specific tasks, which can but don't have to be listed in savegame.
		// We'll sidestep this and assign them jobs they're good at.

		const auto& skills = adviser.second->getSkills();
		std::vector<int> skillList = {skills.learning, skills.stewardship, skills.diplomacy, skills.intrigue, skills.martial};
		const auto& topSkillScore = std::max_element(skillList.begin(), skillList.end());

		if (skills.learning == *topSkillScore)
		{
			newAdviser.type = "theologian";
			newAdviser.skill = std::min(2, std::max(1, adviser.second->getSkills().learning / 4));
		}
		else if (skills.martial == *topSkillScore)
		{
			newAdviser.type = "army_organiser";
			newAdviser.skill = std::min(2, std::max(1, adviser.second->getSkills().martial / 4));
		}
		else if (skills.intrigue == *topSkillScore)
		{
			newAdviser.type = "spymaster";
			newAdviser.skill = std::min(2, std::max(1, adviser.second->getSkills().intrigue / 4));
		}
		else if (skills.stewardship == *topSkillScore)
		{
			newAdviser.type = "treasurer";
			newAdviser.skill = std::min(2, std::max(1, adviser.second->getSkills().stewardship / 4));
		}
		else if (skills.diplomacy == *topSkillScore)
		{
			newAdviser.type = "statesman";
			newAdviser.skill = std::min(2, std::max(1, adviser.second->getSkills().diplomacy / 4));
		}
		else
			continue;

		newAdviser.id = static_cast<int>(adviser.first); // heh.
		newAdviser.appearDate = adviser.second->getBirthDate();
		newAdviser.appearDate.subtractYears(-16);
		newAdviser.deathDate = adviser.second->getBirthDate();
		newAdviser.deathDate.subtractYears(-65);
		newAdviser.female = adviser.second->isFemale();
		if (!adviser.second->getFaith())
		{
			newAdviser.religion = details.monarch.religion; // taking a shortcut.
		}
		else
		{
			const auto& religionMatch =
				 religionMapper.getEU4ReligionForCK3Religion(adviser.second->getFaith()->second->getName(), adviser.second->getFaith()->second->getReligiousHead());
			if (religionMatch)
				newAdviser.religion = *religionMatch;
			else
				newAdviser.religion = details.monarch.religion; // taking a shortcut.
		}
		if (!adviser.second->getCulture())
		{
			newAdviser.culture = details.monarch.culture; // taking a shortcut.
		}
		else
		{
			const auto& cultureMatch = cultureMapper.cultureMatch(adviser.second->getCulture()->second->getName(), newAdviser.religion, 0, tag);
			if (cultureMatch)
				newAdviser.culture = *cultureMatch;
			else
				newAdviser.culture = details.monarch.culture; // taking a shortcut.
		}
		if (newAdviser.religion == "jewish")
			newAdviser.discount = true; // Tradeoff for not being promotable.
		details.advisers.emplace_back(newAdviser);
		adviser.second->setSpent();
	}
}

void EU4::Country::annexCountry(const std::pair<std::string, std::shared_ptr<Country>>& theCountry)
{
	// Provinces
	const auto& targetProvinces = theCountry.second->getProvinces();
	for (const auto& province: targetProvinces)
	{
		// Adding, not replacing. Unless the special snowflake.
		if (tag == "PAP")
			province.second->dropCores();
		province.second->addCore(tag);
		province.second->setOwner(tag);
		province.second->setController(tag);
		provinces.insert(province);
	}
	theCountry.second->clearProvinces();

	// relevant flags
	if (theCountry.second->isHREEmperor())
		details.holyRomanEmperor = true;
	if (theCountry.second->isHREElector())
		details.elector = true;

	// Vassals
	const auto& targetVassals = theCountry.second->getTitle()->second->getGeneratedVassals();
	for (const auto& vassal: targetVassals)
	{
		vassal.second->loadGeneratedLiege(*title);
		title->second->addGeneratedVassal(vassal);
	}
	theCountry.second->getTitle()->second->clearGeneratedVassals();

	// Bricking the title -> eu4tag is not necessary and not desirable. As soon as the country has 0 provinces, it's effectively dead.
}

void EU4::Country::subsumeCountry(const std::shared_ptr<Country>& theCountry)
{
	// Subsuming is the process of both annexing but also becoming the target country. We eat it but we install all of its details as ours.

	auto savedTag = theCountry->getTag();
	auto theDetails = theCountry->getEntireDetails();
	auto theTitle = theCountry->getTitle();
	auto locs = theCountry->getLocalizations();
	auto theConfederationCoa = theCountry->getConfederationCoA();

	annexCountry({savedTag, theCountry});

	// Now become it.

	details = theDetails;
	title = theTitle;
	localizations = locs;
	confederationCoA = theConfederationCoa;

	// Fix the locs.

	if (locs.contains(savedTag))
	{
		if (localizations.contains(tag))
			localizations.at(tag) = locs.at(savedTag);
		else
			localizations.emplace(tag, locs.at(savedTag));
	}

	if (locs.contains(savedTag + "_ADJ"))
	{
		if (localizations.contains(tag + "_ADJ"))
			localizations.at(tag + "_ADJ") = locs.at(savedTag + "_ADJ");
		else
			localizations.emplace(tag + "_ADJ", locs.at(savedTag + "_ADJ"));
	}
}

void EU4::Country::setLocalizations(const mappers::LocBlock& newBlock)
{
	// Setting the name
	localizations[tag] = newBlock;
}

void EU4::Country::correctRoyaltyToBuddhism()
{
	if (details.monarch.religion == "vajrayana")
		details.monarch.religion = "buddhism";
	if (details.queen.religion == "vajrayana")
		details.queen.religion = "buddhism";
	if (details.heir.religion == "vajrayana")
		details.heir.religion = "buddhism";
	for (auto& adviser: details.advisers)
		if (adviser.religion == "vajrayana")
			adviser.religion = "buddhism";
}

date EU4::Country::normalizeDate(const date& incomingDate, Configuration::STARTDATE startDateOption, const date& theConversionDate, const date& startDate) const
{
	if (startDateOption == Configuration::STARTDATE::CK)
		return incomingDate;

	// for 1444 or manual start date bookmark we need to adjust dates. Instead of being anal about days and months, we'll just fix the year so ages match
	// approximately.
	const auto incomingYear = incomingDate.getYear();

	int yearDelta;
	if (startDateOption == Configuration::STARTDATE::EU)
		yearDelta = 1444 - theConversionDate.getYear();
	else
		yearDelta = startDate.getYear() - theConversionDate.getYear();

	return date(incomingYear + yearDelta, incomingDate.getMonth(), incomingDate.getDay());
}

void EU4::Country::renameAndRemask(const CK3::Confederation& confederation)
{
	// Incoming Confederation identity takes over country name, CoA and color. Rest remains as is.
	// Since confederation name is in the save itself, single-language, we don't have an issue with splattering it all over the locs as-is.

	if (!confederation.getName().empty())
	{
		if (localizations.contains(tag))
		{
			localizations.at(tag).initializeAll(confederation.getName());
		}
		else
		{
			mappers::LocBlock name;
			name.initializeAll(confederation.getName());
			localizations.emplace(tag, name);
		}
	}

	// We can semi-create the adjective by stripping Confederation from name, but this works only for english.
	const auto& pos = confederation.getName().find(" Confederation");
	if (pos != std::string::npos && pos > 1)
	{
		const auto baseName = confederation.getName().substr(0, pos);
		if (localizations.contains(tag + "_ADJ"))
		{
			localizations.at(tag + "_ADJ").english = baseName;
		}
		else
		{
			mappers::LocBlock adj;
			adj.initializeAll(baseName);
			localizations.emplace(tag + "_ADJ", adj);
		}
	}

	// Copy over the color.
	if (confederation.getColor())
		details.color = confederation.getColor();

	// And copy over CoA.
	if (confederation.getCoat())
		confederationCoA = confederation.getCoat();
}
