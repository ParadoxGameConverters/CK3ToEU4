#include "Country.h"
#include "../../CK3World/Characters/Character.h"
#include "../../CK3World/Titles/Title.h"
#include "../../Mappers/CultureMapper/CultureMapper.h"
#include "../../Mappers/GovernmentsMapper/GovernmentsMapper.h"
#include "../../Mappers/ProvinceMapper/ProvinceMapper.h"
#include "../../Mappers/RegionMapper/RegionMapper.h"
#include "../../Mappers/ReligionMapper/ReligionMapper.h"
#include "../../Mappers/TraitScraper/TraitScraper.h"
#include "../Province/EU4Province.h"
#include "CommonFunctions.h"
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

void EU4::Country::initializeFromTitle(std::string theTag,
	 std::shared_ptr<CK3::Title> theTitle,
	 const mappers::GovernmentsMapper& governmentsMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const mappers::CultureMapper& cultureMapper,
	 const mappers::ProvinceMapper& provinceMapper,
	 const mappers::ColorScraper& colorScraper,
	 const mappers::LocalizationMapper& localizationMapper,
	 const mappers::TraitScraper& traitScraper,
	 date theConversionDate)
{
	tag = std::move(theTag);
	conversionDate = theConversionDate;
	title.first = theTitle->getName();
	title.second = std::move(theTitle);
	if (commonCountryFile.empty())
		commonCountryFile = "countries/" + title.first + ".txt";
	if (historyCountryFile.empty())
		historyCountryFile = "history/countries/" + tag + " - " + title.first + ".txt";
}
	/*
	const auto& actualHolder = title.second->getHolder()->second;
	if (actualHolder->getDynasty().first)
		details.dynastyID = actualHolder->getDynasty().first;

	// --------------- History section
	details.government.clear();
	details.reforms.clear();
	const auto& newGovernment = governmentsMapper.matchGovernment(actualHolder->getGovernment(), title.first);
	if (newGovernment)
	{
		details.government = newGovernment->first;
		if (!newGovernment->second.empty())
			details.reforms.insert(newGovernment->second);
	}
	else
	{
		Log(LogLevel::Warning) << "No government match for " << actualHolder->getGovernment() << " for title: " << title.first << ", defaulting to monarchy.";
		details.government = "monarchy";
	}
	if (title.first.find("e_") == 0)
		details.governmentRank = 3;
	else if (title.first.find("k_") == 0)
		details.governmentRank = 2;
	else
		details.governmentRank = 1;
	// Reforms will be set later to ensure that all other aspects of a country have been correctly set first
	// do we have a religion?
	std::string baseReligion;
	if (!actualHolder->getReligion().empty())
		baseReligion = actualHolder->getReligion();
	else
		baseReligion = actualHolder->getCapitalProvince().second->getReligion();
	const auto& religionMatch = religionMapper.getEU4ReligionForCK3Religion(baseReligion);
	if (religionMatch)
		details.religion = *religionMatch;
	else
	{
		// We failed to get a religion. This is not an issue. We'll set it later from the majority of owned provinces.
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
	} // We will set it later if primaryCulture is unavailable at this stage.
	if (!details.color)
	{
		if (title.second->getColor())
			details.color = title.second->getColor();
		else
		{
			const auto& colorMatch = colorScraper.getColorForTitle(title.first);
			if (colorMatch)
				details.color = *colorMatch;
			else
				Log(LogLevel::Warning) << tag << ": No color defined for title " << title.first << "!";
		}
	}
	// If we imported some revolutionary colors we'll keep them, otherwise we're ignoring this.
	// If we imported historical idea groups, keeping them, otherwise blank.
	details.randomChance = false; // random chance related to RNW, wo it has no effect here.

	// If we imported historical units, keeping them, otherwise blank.

	details.monarchNames.clear();
	if (!title.second->getPreviousHolders().empty())
	{
		for (const auto& previousHolder: title.second->getPreviousHolders())
		{
			const auto& blockItr = details.monarchNames.find(previousHolder.second->getName());
			if (blockItr != details.monarchNames.end())
				blockItr->second.first++;
			else
			{
				auto female = previousHolder.second->isFemale();
				auto chance = 10;
				if (female)
					chance = -1;
				std::pair<int, int> newBlock = std::pair(1, chance);
				details.monarchNames.insert(std::pair(previousHolder.second->getName(), newBlock));
			}
		}
	}
	if (!title.second->getHolder().second->getCourtierNames().empty())
	{
		for (const auto& courtier: title.second->getHolder().second->getCourtierNames())
		{
			const auto& blockItr = details.monarchNames.find(courtier.first);
			if (blockItr == details.monarchNames.end())
			{
				auto female = !courtier.second;
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

	// --------------  Misc

	if (actualHolder->getWealth())
		details.addTreasury = lround(7 * log2(actualHolder->getWealth()));
	if (actualHolder->getPrestige())
		details.addPrestige = -50 + std::max(-50, static_cast<int>(lround(15 * log2(actualHolder->getPrestige() - 100) - 50)));
	if (actualHolder->hasLoan())
		details.loan = true;
	if (actualHolder->hasTrait("excommunicated"))
		details.excommunicated = true;

	// ------------------ Country Name Locs

	auto nameSet = false;

	// Pope is special, as always.
	if (title.second->isThePope() && tag == "PAP")
		nameSet = true; // We'll use vanilla PAP locs.
	else if (title.second->isTheFraticelliPope())
	{
		auto nameLocalizationMatch = localizationMapper.getLocBlockForKey("d_fraticelli");
		if (nameLocalizationMatch)
		{
			localizations.insert(std::pair(tag, *nameLocalizationMatch));
			nameSet = true;
		}
	}

	if (!nameSet && !title.second->getDisplayName().empty())
	{
		mappers::LocBlock newblock;
		newblock.english = title.second->getDisplayName();
		newblock.spanish = title.second->getDisplayName();
		newblock.french = title.second->getDisplayName();
		newblock.german = title.second->getDisplayName();
		localizations.insert(std::pair(tag, newblock));
		nameSet = true;
	}
	if (!nameSet)
	{
		auto nameLocalizationMatch = localizationMapper.getLocBlockForKey(title.first);
		if (nameLocalizationMatch)
		{
			localizations.insert(std::pair(tag, *nameLocalizationMatch));
			nameSet = true;
		}
	}
	if (!nameSet && !title.second->getBaseTitle().first.empty())
	{ // see if we can match vs base title.
		auto baseTitleName = title.second->getBaseTitle().first;
		auto nameLocalizationMatch = localizationMapper.getLocBlockForKey(baseTitleName);
		if (nameLocalizationMatch)
		{
			localizations.insert(std::pair(tag, *nameLocalizationMatch));
			nameSet = true;
		}
	}
	if (!nameSet)
	{
		// Now get creative. This happens for c_titles that have localizations as b_title
		auto alternateName = title.second->getName();
		alternateName = "b_" + alternateName.substr(2, alternateName.length());
		auto nameLocalizationMatch = localizationMapper.getLocBlockForKey(alternateName);
		if (nameLocalizationMatch)
		{
			localizations.insert(std::pair(tag, *nameLocalizationMatch));
			nameSet = true;
		}
	}
	if (!nameSet)
	{
		// using capital province name?
		auto capitalName = actualHolder->getCapitalProvince().second->getName();
		if (!capitalName.empty())
		{
			mappers::LocBlock newblock;
			newblock.english = capitalName;
			newblock.spanish = capitalName;
			newblock.french = capitalName;
			newblock.german = capitalName;
			localizations.insert(std::pair(tag, newblock));
			nameSet = true;
		}
	}

	// Override for kingdoms/empires that use Dynasty Names
	std::set<std::string> dynastyTitleNames = {"turkmeni", "khazak", "uzbehk", "turkish",
		 "karluk",
		 "khitan",
		 "tuareg", "frencharab", "andalucian", "hejazi_culture", "gulf_arabic", "mahri_culture", "al_iraqiya_arabic", "omani_culture", "yemeni_culture",
		 "bedouin_arabic", "algerian", "moroccan", "tunisian"
		 "berber", "maghreb_arabic",
		 "al_suryah_arabic", "levantine_arabic",
		 "al_misr_arabic", "egyptian_arabic",
		 "andalusian_arabic",
		 "azerbaijani", "khorasani", "mazandarani", "luri", "tajik", "persian",
		 "kurdish",
		 "afghani", "afghan",
		 "baluchi", "baloch",
		 "bihari", "kochi", "bengali",
		 "oriya",
		 "assamese",
		 "pahari", "kanauji", "vindhyan", "avadhi", "hindustani",
		 "saurashtri", "gujarati", "gujurati",
		 "kashmiri", "panjabi",
		 "malvi", "rajput",
		 "sindhi",
		 "marathi",
		 "sinhala",
		 "malayalam", "tamil",
		 "telegu", "telugu",
		 "kannada"};

	std::set<std::string> hardcodedExclusions =
		 {"k_rum", "k_israel", "e_india", "e_il-khanate", "e_persia", "e_mali", "k_mali", "k_ghana", "k_songhay", "e_hre", "e_rome", "e_byzantium"};

	if (details.government == "monarchy" && dynastyTitleNames.count(details.primaryCulture) && actualHolder->getDynasty().first &&
		 !actualHolder->getDynasty().second->getName().empty() && !hardcodedExclusions.count(title.first) &&
		 (title.first.find("e_") == 0 || title.first.find("k_") == 0))
	{
		const auto& dynastyName = actualHolder->getDynasty().second->getName();
		mappers::LocBlock newblock;
		if (dynastyName.back() == 's')
			newblock.english = dynastyName;
		else
			newblock.english = dynastyName + "s";
		newblock.spanish = dynastyName;
		newblock.french = dynastyName;
		newblock.german = dynastyName;

		// If we already set a canonical name, don't just overwrite, save it for future reference (Ottoman Crimea)
		if (nameSet)
		{
			localizations.insert(std::pair("canonical", localizations[tag]));
		}

		localizations[tag] = newblock;
		details.hasDynastyName = true;
		nameSet = true;
	}

	// giving up.
	if (!nameSet)
		Log(LogLevel::Warning) << tag << " help with localization! " << title.first;

	// --------------- Adjective Locs

	auto adjSet = false;

	// Pope is special, as always.
	if (title.second->isThePope())
		adjSet = true; // We'll use vanilla PAP locs.
	else if (title.second->isTheFraticelliPope())
	{
		auto adjLocalizationMatch = localizationMapper.getLocBlockForKey("d_fraticelli_adj");
		if (adjLocalizationMatch)
		{
			localizations.insert(std::pair(tag + "_ADJ", *adjLocalizationMatch));
			adjSet = true;
		}
	}

	if (!adjSet && dynastyTitleNames.count(details.primaryCulture) && actualHolder->getDynasty().first &&
		 !actualHolder->getDynasty().second->getName().empty() && title.first != "k_rum" && title.first != "k_israel" && title.first != "e_india" &&
		 (title.first.find("e_") == 0 || title.first.find("k_") == 0))
	{
		const auto& dynastyName = actualHolder->getDynasty().second->getName();
		mappers::LocBlock newblock;
		newblock.english = dynastyName; // Ottoman Africa
		newblock.spanish = "de los " + dynastyName;
		newblock.french = "des " + dynastyName;
		newblock.german = dynastyName + "-";
		localizations.insert(std::pair(tag + "_ADJ", newblock));
		adjSet = true;
	}
	if (!adjSet && !title.second->getDisplayName().empty())
	{
		mappers::LocBlock newblock;
		newblock.english = title.second->getDisplayName() + "'s"; // singular Nordarike's Africa
		newblock.spanish = "de " + title.second->getDisplayName();
		newblock.french = "de " + title.second->getDisplayName();
		newblock.german = title.second->getDisplayName() + "s";
		localizations.insert(std::pair(tag + "_ADJ", newblock));
		adjSet = true;
	}
	if (!adjSet)
	{
		auto adjLocalizationMatch = localizationMapper.getLocBlockForKey(title.first + "_adj");
		if (adjLocalizationMatch)
		{
			localizations.insert(std::pair(tag + "_ADJ", *adjLocalizationMatch));
			adjSet = true;
		}
	}
	if (!adjSet && !title.second->getBaseTitle().first.empty())
	{
		// see if we can match vs base title.
		auto baseTitleAdj = title.second->getBaseTitle().first + "_adj";
		auto adjLocalizationMatch = localizationMapper.getLocBlockForKey(baseTitleAdj);
		if (adjLocalizationMatch)
		{
			localizations.insert(std::pair(tag + "_ADJ", *adjLocalizationMatch));
			adjSet = true;
		}
		if (!adjSet && !title.second->getBaseTitle().second->getBaseTitle().first.empty())
		{
			// maybe basetitlebasetitle?
			baseTitleAdj = title.second->getBaseTitle().second->getBaseTitle().first + "_adj";
			adjLocalizationMatch = localizationMapper.getLocBlockForKey(baseTitleAdj);
			if (adjLocalizationMatch)
			{
				localizations.insert(std::pair(tag + "_ADJ", *adjLocalizationMatch));
				adjSet = true;
			}
		}
	}
	if (!adjSet)
	{
		// Maybe c_something?
		auto alternateAdj = title.second->getName() + "_adj";
		alternateAdj = "c_" + alternateAdj.substr(2, alternateAdj.length());
		auto adjLocalizationMatch = localizationMapper.getLocBlockForKey(alternateAdj);
		if (adjLocalizationMatch)
		{
			localizations.insert(std::pair(tag, *adjLocalizationMatch));
			adjSet = true;
		}
	}
	if (!adjSet)
	{
		// Or d_something?
		auto alternateAdj = title.second->getName() + "_adj";
		alternateAdj = "d_" + alternateAdj.substr(2, alternateAdj.length());
		auto adjLocalizationMatch = localizationMapper.getLocBlockForKey(alternateAdj);
		if (adjLocalizationMatch)
		{
			localizations.insert(std::pair(tag, *adjLocalizationMatch));
			adjSet = true;
		}
	}
	if (!adjSet)
		Log(LogLevel::Warning) << tag << " help with localization for adjective! " << title.first << "_adj?";

	// Rulers
	initializeRulers(religionMapper, cultureMapper, traitScraper);
}

void EU4::Country::setLocalizations(const mappers::LocBlock& newBlock)
{
	// Setting the name
	localizations[tag] = newBlock;
}

bool EU4::Country::verifyCapital(const mappers::ProvinceMapper& provinceMapper)
{
	// We have set a provisionary capital earlier, but now we can check if it's valid.
	if (title.first.empty())
		return false;
	if (provinces.empty())
		return false;
	if (details.capital && provinces.count(details.capital))
		return false;

	const auto& actualHolder = title.second->getHolder().second;
	const auto& capitalMatch = provinceMapper.getEU4ProvinceNumbers(actualHolder->getCapitalProvince().first);
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


void EU4::Country::initializeAdvisers(const mappers::ReligionMapper& religionMapper, const mappers::CultureMapper& cultureMapper)
{
	// We're doing this one separate from initial country generation so that country's primary culture and religion may have had time to get
	// initialized.
	if (title.first.empty() || !title.second->getHolder().first)
		return; // Vanilla and the dead do not get these.
	const auto& holder = title.second->getHolder().second;
	if (!holder->getPrimaryTitle().first.empty() && title.first != holder->getPrimaryTitle().first)
		return; // PU's don't get advisors on secondary countries.

	for (const auto& adviser: holder->getAdvisers())
	{
		if (adviser.second->isSpent())
			continue;
		Character newAdviser;
		newAdviser.name = adviser.second->getName();
		if (adviser.second->getDynasty().first)
			newAdviser.name += " " + adviser.second->getDynasty().second->getName();
		if (details.capital)
			newAdviser.location = details.capital;
		if (adviser.second->getJob() == "job_spiritual")
		{
			newAdviser.type = "theologian";
			newAdviser.skill = std::min(3, std::max(1, adviser.second->getSkills().learning / 4));
		}
		else if (adviser.second->getJob() == "job_marshal")
		{
			newAdviser.type = "army_organiser";
			newAdviser.skill = std::min(3, std::max(1, adviser.second->getSkills().martial / 4));
		}
		else if (adviser.second->getJob() == "job_spymaster")
		{
			newAdviser.type = "spymaster";
			newAdviser.skill = std::min(3, std::max(1, adviser.second->getSkills().intrigue / 4));
		}
		else if (adviser.second->getJob() == "job_treasurer")
		{
			newAdviser.type = "treasurer";
			newAdviser.skill = std::min(3, std::max(1, adviser.second->getSkills().intrigue / 4));
		}
		else if (adviser.second->getJob() == "job_chancellor")
		{
			newAdviser.type = "statesman";
			newAdviser.skill = std::min(3, std::max(1, adviser.second->getSkills().diplomacy / 4));
		}
		else
		{
			Log(LogLevel::Warning) << "Unrecognized job for " << adviser.first << ": " << adviser.second->getJob();
			continue;
		}
		newAdviser.id = adviser.first;
		newAdviser.appearDate = adviser.second->getBirthDate();
		newAdviser.appearDate.subtractYears(-16);
		newAdviser.deathDate = adviser.second->getBirthDate();
		newAdviser.deathDate.subtractYears(-65);
		newAdviser.female = adviser.second->isFemale();
		if (adviser.second->getReligion().empty())
			newAdviser.religion = details.monarch.religion; // taking a shortcut.
		else
		{
			const auto& religionMatch = religionMapper.getEu4ReligionForCk2Religion(adviser.second->getReligion());
			if (religionMatch)
				newAdviser.religion = *religionMatch;
		}
		if (newAdviser.religion.empty())
			continue;
		if (adviser.second->getCulture().empty())
			newAdviser.culture = details.monarch.culture; // taking a shortcut.
		else
		{
			const auto& cultureMatch = cultureMapper.cultureMatch(adviser.second->getCulture(), newAdviser.religion, 0, tag);
			if (cultureMatch)
				newAdviser.culture = *cultureMatch;
		}
		if (newAdviser.culture.empty())
			continue;
		if (newAdviser.religion == "jewish")
			newAdviser.discount = true; // Tradeoff for not being promotable.
		details.advisers.emplace_back(newAdviser);
		adviser.second->setSpent();
	}
}


void EU4::Country::initializeRulers(const mappers::ReligionMapper& religionMapper,
	 const mappers::CultureMapper& cultureMapper,
	 const mappers::RulerPersonalitiesMapper& rulerPersonalitiesMapper)
{
	const auto& holder = title.second->getHolder().second;
	// Are we the ruler's primary title? (if he has any)
	// Potential PU's don't get monarchs. (and those apply for monarchies only)
	if (!holder->getPrimaryTitle().first.empty() && title.first != holder->getPrimaryTitle().first && details.government == "monarchy")
		return; 

	// Determine regnalness.
	if (details.government != "republic" && !details.monarchNames.empty())
	{
		auto const& theName = holder->getName();
		std::string roman;
		const auto& nameItr = details.monarchNames.find(theName);
		if (nameItr != details.monarchNames.end())
		{
			const auto regnal = nameItr->second.first;
			if (regnal > 1)
			{
				roman = cardinalToRoman(regnal);
				roman = " " + roman;
			}
		}
		details.monarch.name = holder->getName() + roman;
	}
	else
	{
		details.monarch.name = holder->getName();
	}
	if (holder->getDynasty().first)
		details.monarch.dynasty = holder->getDynasty().second->getName();
	details.monarch.adm = std::min((holder->getSkills().stewardship + holder->getSkills().learning) / 3, 6);
	details.monarch.dip = std::min((holder->getSkills().diplomacy + holder->getSkills().intrigue) / 3, 6);
	details.monarch.mil = std::min((holder->getSkills().martial + holder->getSkills().learning) / 3, 6);
	details.monarch.birthDate = holder->getBirthDate();
	details.monarch.female = holder->isFemale();
	// religion and culture were already determining our country's primary culture and religion. If we set there, we'll copy here.
	if (!details.primaryCulture.empty())
		details.monarch.culture = details.primaryCulture;
	if (!details.religion.empty())
		details.monarch.religion = details.religion;
	details.monarch.personalities = rulerPersonalitiesMapper.evaluatePersonalities(title.second->getHolder());
	details.monarch.isSet = true;

	if (!holder->getSpouses().empty())
	{
		// What's the first spouse that's still alive?
		for (const auto& spouse: holder->getSpouses())
		{
			if (spouse.second->getDeathDate() != date("1.1.1"))
				continue; // She's dead.
			details.queen.name = spouse.second->getName();
			if (spouse.second->getDynasty().first)
				details.queen.dynasty = spouse.second->getDynasty().second->getName();
			details.queen.adm = std::min((spouse.second->getSkills().stewardship + spouse.second->getSkills().learning) / 3, 6);
			details.queen.dip = std::min((spouse.second->getSkills().diplomacy + spouse.second->getSkills().intrigue) / 3, 6);
			details.queen.mil = std::min((spouse.second->getSkills().martial + spouse.second->getSkills().learning) / 3, 6);
			details.queen.birthDate = spouse.second->getBirthDate();
			details.queen.female = spouse.second->isFemale();
			if (spouse.second->getReligion().empty())
				details.queen.religion = details.monarch.religion; // taking a shortcut.
			else
			{
				const auto& religionMatch = religionMapper.getEu4ReligionForCk2Religion(spouse.second->getReligion());
				if (religionMatch)
					details.queen.religion = *religionMatch;
			}
			if (spouse.second->getCulture().empty())
				details.queen.culture = details.monarch.culture; // taking a shortcut.
			else
			{
				const auto& cultureMatch = cultureMapper.cultureMatch(spouse.second->getCulture(), details.queen.religion, 0, tag);
				if (cultureMatch)
					details.queen.culture = *cultureMatch;
			}
			details.queen.originCountry = tag;
			details.queen.deathDate = details.queen.birthDate;
			details.queen.deathDate.subtractYears(-60);
			details.queen.personalities = rulerPersonalitiesMapper.evaluatePersonalities(spouse);
			details.queen.isSet = true;
			break;
		}
	}

	if (holder->getHeir().first)
	{
		const auto& heir = holder->getHeir();
		details.heir.name = heir.second->getName();
		// We're setting future regnalness
		if (details.government != "republic" && !details.monarchNames.empty())
		{
			auto const& theName = heir.second->getName();
			std::string roman;
			const auto& nameItr = details.monarchNames.find(theName);
			if (nameItr != details.monarchNames.end())
			{
				const auto regnal = nameItr->second.first;
				if (regnal >= 1)
				{
					roman = cardinalToRoman(regnal + 1);
					roman = " " + roman;
				}
			}
			details.heir.monarchName = heir.second->getName() + roman;
		}
		if (heir.second->getDynasty().first)
			details.heir.dynasty = heir.second->getDynasty().second->getName();
		details.heir.adm = std::min((heir.second->getSkills().stewardship + heir.second->getSkills().learning) / 2, 6);
		details.heir.dip = std::min((heir.second->getSkills().diplomacy + heir.second->getSkills().intrigue) / 2, 6);
		details.heir.mil = std::min((heir.second->getSkills().martial + heir.second->getSkills().learning) / 2, 6);
		details.heir.birthDate = heir.second->getBirthDate();
		details.heir.female = heir.second->isFemale();
		if (heir.second->getReligion().empty())
			details.heir.religion = details.monarch.religion; // taking a shortcut.
		else
		{
			const auto& religionMatch = religionMapper.getEu4ReligionForCk2Religion(heir.second->getReligion());
			if (religionMatch)
				details.heir.religion = *religionMatch;
		}
		if (heir.second->getCulture().empty())
			details.heir.culture = details.monarch.culture; // taking a shortcut.
		else
		{
			const auto& cultureMatch = cultureMapper.cultureMatch(heir.second->getCulture(), details.heir.religion, 0, tag);
			if (cultureMatch)
				details.heir.culture = *cultureMatch;
		}
		details.heir.deathDate = details.heir.birthDate;
		details.heir.deathDate.subtractYears(-65);
		details.heir.claim = 89; // good enough?
		details.heir.personalities = rulerPersonalitiesMapper.evaluatePersonalities(heir);
		details.heir.isSet = true;
	}

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

int EU4::Country::getDevelopment() const
{
	auto dev = 0;
	for (const auto& province: provinces)
		dev += province.second->getDev();
	return dev;
}

void EU4::Country::annexCountry(const std::pair<std::string, std::shared_ptr<Country>>& theCountry)
{
	// Provinces
	const auto& targetProvinces = theCountry.second->getProvinces();
	for (const auto& province: targetProvinces)
	{
		// Adding, not replacing. Unless the special snowflake.
		if (tag == "PAP" || tag == "FAP")
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
	const auto& targetVassals = theCountry.second->getTitle().second->getGeneratedVassals();
	for (const auto& vassal: targetVassals)
	{
		vassal.second->registerGeneratedLiege(title);
		title.second->registerGeneratedVassal(vassal);
	}
	theCountry.second->getTitle().second->clearGeneratedVassals();

	// Bricking the title -> eu4tag is not necessary and not desirable. As soon as the country has 0 provinces, it's effectively dead.
}

void EU4::Country::assignReforms(std::shared_ptr<mappers::RegionMapper> regionMapper)
{
	// Setting the Primary Religion (The religion most common in the country, not the religion of the country, needed for some reforms)
	if (details.majorityReligion.empty() || details.majorityReligion == "noreligion")
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

	const auto& actualHolder = title.second->getHolder().second;
	bool isMerc = false;

	// Checking to see if you have a center of trade with level 2 or higher
	bool hasTradeCenterLevelTwo = false;
	for (const auto& province: provinces)
	{
		if (province.second->getCenterOfTradeLevel() >= 2)
		{
			hasTradeCenterLevelTwo = true;
			break;
		}
	}

	// Muslims
	std::set<std::string> muslimReligions = {"sunni", "zikri", "yazidi", "ibadi", "kharijite", "shiite", "druze", "hurufi", "qarmatian"};
	// Mazdans
	std::set<std::string> mazdanReligions = {"zoroastrian", "mazdaki", "manichean", "khurmazta"};
	// Buddhists
	std::set<std::string> buddhistReligions = {"buddhism", "vajrayana", "mahayana"};
	// Eastern
	std::set<std::string> easternReligions = {"confucianism", "shinto", "buddhism", "vajrayana", "mahayana"};
	// Indians (Dharmic + Buddhists)
	std::set<std::string> indianReligions = {"buddhism", "vajrayana", "mahayana", "hinduism", "jain"};
	// Protestants
	std::set<std::string> protestantReligions = {"protestant", "reformed", "hussite", "cathar", "waldensian", "lollard"};
	// Orthodox
	std::set<std::string> orthodoxReligions = {"orthodox", "monothelite", "iconoclast", "paulician", "bogomilist"};
	// Pagan
	std::set<std::string> paganReligions = {"pagan_religion",
		 "norse_pagan",
		 "norse_pagan_reformed",
		 "tengri_pagan",
		 "tengri_pagan_reformed",
		 "baltic_pagan",
		 "baltic_pagan_reformed",
		 "finnish_pagan",
		 "finnish_pagan_reformed",
		 "slavic_pagan",
		 "slavic_pagan_reformed",
		 "shamanism",
		 "west_african_pagan",
		 "west_african_pagan_reformed",
		 "hellenic_pagan",
		 "hellenic_pagan_reformed",
		 "zun_pagan",
		 "zun_pagan_reformed",
		 "bon",
		 "bon_reformed",
		 "animism",
		 "totemism",
		 "inti",
		 "nahuatl",
		 "mesoamerican_religion"};

	// Russian Cultures (Not all East Slavic)
	std::set<std::string> russianCultures = {"ilmenian", "volhynian", "severian", "russian", "russian_culture", "novgorodian", "ryazanian"};
	// Dravidian Culture Group
	std::set<std::string> dravidianCultures = {"kannada", "malayalam", "tamil", "telegu"};
	// Western Aryan Culture Group
	std::set<std::string> westAryanCultures = {"gujarati", "saurashtri", "marathi", "sindhi", "rajput", "malvi"};
	// Baltic Culture Group
	std::set<std::string> balticCultures = {"estonian", "lithuanian", "latvian", "old_prussian"};
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
	// GENERIC REFORMS
	std::set<std::string> laws = title.second->getLaws();
	std::string governmentType = "despotic"; // Despotism will be the default
	short numberOfLaws = 0;
	// These are the council laws that give power to the council. The ones that give power to the monarch would end in 0 instead of 1
	if (title.second->getLaws().count("law_voting_power_1"))
		numberOfLaws++;
	if (title.second->getLaws().count("banish_voting_power_1"))
		numberOfLaws++;
	if (title.second->getLaws().count("execution_voting_power_1"))
		numberOfLaws++;
	if (title.second->getLaws().count("revoke_title_voting_power_1"))
		numberOfLaws++;
	if (title.second->getLaws().count("grant_title_voting_power_1"))
		numberOfLaws++;
	if (title.second->getLaws().count("imprison_voting_power_1"))
		numberOfLaws++;
	if (title.second->getLaws().count("war_voting_power_1"))
		numberOfLaws++;
	if (numberOfLaws >= 6)
		governmentType = "aristocratic";
	else if (numberOfLaws >= 2 && numberOfLaws < 7)
		governmentType = "despotic";
	else
		governmentType = "absolute";

	// MONARCHIES
	if (details.government == "monarchy")
	{
		// Electoral
		if (title.second->getSuccessionLaw() == "feudal_elective" && tag != "ROM" && tag != "HRE" && tag != "BYZ")
		{
			details.reforms.clear();
			details.reforms = {"elective_monarchy"};
		}
		// Weird Edge Cases
		else if (title.second->getSuccessionLaw() == "byzantine_elective" && (title.first.find("e_roman_empire") || title.first.find("e_byzantium")))
		{
			details.reforms.clear();
			details.reforms = {"autocracy_reform"};
		}
		else if (title.second->getSuccessionLaw() == "byzantine_elective" && !title.first.find("e_"))
		{
			details.reforms.clear();
			details.reforms = {"feudalism_reform"};
		}
		else if (title.second->getSuccessionLaw() == "open_elective") // Should only be applicable to Mercenary Companies
		{
			details.government.clear();
			details.government = "republic";
			details.reforms.clear();
			details.reforms = {"noble_elite_reform"};
			isMerc = true;
		}
		else if (actualHolder->getGovernment() == "roman_imperial_government" || actualHolder->getGovernment() == "chinese_imperial_government")
		{
			details.reforms.clear();
			details.reforms = {"autocracy_reform"};
		}
		else if (actualHolder->getGovernment() == "theocratic_feudal_government")
		{
			details.reforms.clear();
			details.reforms = {"feudal_theocracy"};
		}
		// Iqta
		else if (actualHolder->getGovernment() == "muslim_government" && muslimReligions.count(details.religion) &&
					(governmentType == "aristocratic" || governmentType == "despotic"))
		{
			details.reforms.clear();
			details.reforms = {"iqta"};
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
		// Ottoman Government (Renamed in converter)
		else if (actualHolder->getGovernment() == "muslim_government" && muslimReligions.count(details.religion) && governmentType == "absolute")
		{
			details.reforms.clear();
			details.reforms = {"ottoman_government"};
		}
		// Autocracy, also the fallback
		else // if (governmentType == "absolute")
		{
			details.reforms.clear();
			details.reforms = {"autocracy_reform"};
		}
	}
	// REPUBLICS
	if (details.government == "republic" && !details.reforms.count("noble_elite_reform"))
	{
		// Weird Edge Cases
		if (actualHolder->getGovernment() == "confucian_bureaucracy")
		{
			details.government.clear();
			details.government = "monarchy";
			details.reforms.clear();
			details.reforms = {"autocracy_reform"};
		}
		// Merchant Republic
		else if (actualHolder->getGovernment() == "merchant_republic_government")
		{
			details.reforms.clear();
			details.reforms = {"merchants_reform"};
			details.mercantilism = 25;
		}
		// Oligarchic Republic
		else if (actualHolder->getGovernment() == "republic_government" && governmentType != "aristocratic")
		{
			details.reforms.clear();
			details.reforms = {"oligarchy_reform"};
		}
		// Noble Elites, also the fallback
		else
		{
			details.reforms.clear();
			details.reforms = {"noble_elite_reform"};
		}
	}
	// TRIBES
	if (details.government == "tribal")
	{
		// Tribal Kingdoms
		if (title.second->getSuccessionLaw() == "gavelkind")
		{
			details.reforms.clear();
			details.reforms = {"tribal_kingdom"};
		}
		// Hordes
		else if (actualHolder->getGovernment() == "nomadic_government")
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
		// Tribal Federations
		else if (muslimReligions.count(details.religion))
		{
			details.reforms.clear();
			details.reforms = {"tribal_federation"};
		}
		// Tribal Despotism, also the fallback
		else
		{
			details.reforms.clear();
			details.reforms = {"tribal_despotism"};
		}
	}
	// THEOCRACIES
	if (details.government == "theocracy")
	{
		// Papacy
		if (tag == "PAP" || tag == "FAP" || title.first.find("k_papal_state") == 0 || title.first.find("d_fraticelli") == 0)
		{
			details.reforms.clear();
			details.reforms = {"papacy_reform"};
		}
		// Holy Orders
		else if (title.second->getSuccessionLaw() == "open_elective")
		{
			details.reforms.clear();
			details.reforms = {"monastic_order_reform"};
		}
		// Crusader States
		else if ((details.religion == "catholic" || details.religion == "fraticelli") && (tag == "KOJ" || tag == "EGY"))
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

	// Mughal Diwan System is a LEVEL 2 REFORM


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
				(tag == "UKR" || (tag == "RUS" && (orthodoxReligions.count(details.religion) || details.religion == "slavic_pagan" ||
																  details.religion == "slavic_pagan_reformed"))))
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
	else if (muslimReligions.count(details.religion) && isMerc &&
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
	// Peasant Republic
	else if (details.government != "theocracy" && tag != "ROM" && tag != "HRE" && tag != "BYZ" && actualHolder->hasTrait("peasant_leader") &&
				(provinces.size() == 1 || details.government == "republic"))
	{
		details.government.clear();
		details.government = "republic";
		details.reforms.clear();
		details.reforms = {"peasants_republic"};
	}
	// Free City (HRE) - These have already been set in EU4World.cpp by the setFreeCities() method

	// Veche Republic
	else if (details.government == "republic" && details.governmentRank != 3 &&
				(orthodoxReligions.count(details.religion) || details.religion == "slavic_pagan" || details.religion == "slavic_pagan_reformed") &&
				russianCultures.count(details.primaryCulture) && tag != "POL" && tag != "PAP" && tag != "HLR")
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
	// Dutch Republic
	else if (details.government == "republic" && tag == "NED")
	{
		details.reforms.clear();
		details.reforms = {"dutch_republic"};
		details.mercantilism = 15;
	}
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
*/