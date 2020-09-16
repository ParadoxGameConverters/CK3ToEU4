#include "EU4Province.h"
#include "../../CK3World/Characters/Character.h"
#include "../../CK3World/Cultures/Culture.h"
#include "../../CK3World/Geography/CountyDetail.h"
#include "../../CK3World/Religions/Faith.h"
#include "../../CK3World/Titles/LandedTitles.h"
#include "../../CK3World/Titles/Title.h"
#include "../../Mappers/CultureMapper/CultureMapper.h"
#include "../../Mappers/ReligionMapper/ReligionMapper.h"
#include "../Country/Country.h"
#include "Log.h"

EU4::Province::Province(int id, const std::string& filePath): provID(id)
{
	// Load from a country file, if one exists. Otherwise rely on defaults.
	const auto startPos = filePath.find("/history");
	historyProvincesFile = filePath.substr(startPos + 1, filePath.length() - startPos);
	details = ProvinceDetails(filePath);
}

void EU4::Province::updateWith(const std::string& filePath)
{
	// We're doing this for special reason and from a specific source.
	details.updateWith(filePath);
}

void EU4::Province::initializeFromCK3Title(const std::shared_ptr<CK3::Title>& origProvince,
	 const mappers::CultureMapper& cultureMapper,
	 const mappers::ReligionMapper& religionMapper)
{
	if (provID == 1115)
		Log(LogLevel::Debug) << 1;
	srcProvince = origProvince;

	details.discoveredBy = {"eastern", "western", "muslim", "ottoman", "indian", "nomad_group"}; // hardcoding for now.

	// If we're initializing this from CK3 full-fledged titles, and having a holder and development is a given.
	// There are no uncolonized provinces in CK3.
	if (provID == 1115)
		Log(LogLevel::Debug) << 2;

	if (!srcProvince->getHoldingTitle().second->getEU4Tag())
		throw std::runtime_error(
			 "EU4 Province ID " + std::to_string(provID) + " has source holdingtitle " + srcProvince->getHoldingTitle().first + " which has no EU4 tag!");
	tagCountry = *srcProvince->getHoldingTitle().second->getEU4Tag(); // linking to our holder
	details.owner = tagCountry.first;
	details.controller = tagCountry.first;
	if (provID == 1115)
		Log(LogLevel::Debug) << 3;

	// History section
	// Not touching Capital, that's hardcoded English name.
	details.isCity = true; // This must be true for all incoming provinces.

	// Religion first.
	auto religionSet = false;
	auto baseReligion = srcProvince->getClay()->getCounty()->second->getFaith().second->getName();
	if (provID == 1115)
		Log(LogLevel::Debug) << 4;
	auto religionMatch = religionMapper.getEU4ReligionForCK3Religion(baseReligion);
	if (religionMatch)
	{
		details.religion = *religionMatch;
		religionSet = true;
	}
	else
	{
		Log(LogLevel::Warning) << "CK3 Faith " << baseReligion << " for EU4 province " << provID << " has no mapping! Substituting holder's.";
	}
	if (provID == 1115)
		Log(LogLevel::Debug) << 5;

	// Attempt to use religion of ruler in THAT province.
	if (!religionSet)
	{
		baseReligion = srcProvince->getHolder()->second->getFaith().second->getName();
		religionMatch = religionMapper.getEU4ReligionForCK3Religion(baseReligion);
		if (religionMatch)
		{
			details.religion = *religionMatch;
			religionSet = true;
		}
		else
		{
			Log(LogLevel::Warning) << "CK3 Faith " << baseReligion << " for holder of EU4 province " << provID << " has no mapping! Substituting TAG's.";
		}
	}
	if (provID == 1115)
		Log(LogLevel::Debug) << 6;
	// Attempt to use religion of country.
	if (!religionSet)
	{
		if (!tagCountry.second->getReligion().empty())
			details.religion = tagCountry.second->getReligion();
		else
			Log(LogLevel::Warning) << "EU4 " << tagCountry.first << " has no religion set! Defaulting.";
	}
	if (provID == 1115)
		Log(LogLevel::Debug) << 7;

	auto cultureSet = false;
	// do we even have a base culture?
	auto baseCulture = srcProvince->getClay()->getCounty()->second->getCulture().second->getName();
	if (provID == 1115)
		Log(LogLevel::Debug) << 8;
	auto cultureMatch = cultureMapper.cultureMatch(baseCulture, details.religion, provID, tagCountry.first);
	if (cultureMatch)
	{
		details.culture = *cultureMatch;
		cultureSet = true;
	}
	else
	{
		Log(LogLevel::Warning) << "CK3 Culture " << baseCulture << " for EU4 province " << provID << " has no mapping! Substituting holder's.";
	}
	// Attempt to use primary culture of ruler in THAT province.
	if (provID == 1115)
		Log(LogLevel::Debug) << 9;
	if (!cultureSet)
	{
		baseCulture = srcProvince->getHolder()->second->getCulture().second->getName();
		cultureMatch = cultureMapper.cultureMatch(baseCulture, details.religion, provID, tagCountry.first);
		if (cultureMatch)
		{
			details.culture = *cultureMatch;
			cultureSet = true;
		}
		else
		{
			Log(LogLevel::Warning) << "CK3 Culture " << baseCulture << " for holder of EU4 province " << provID << " has no mapping! Substituting TAG's.";
		}
	}
	if (provID == 1115)
		Log(LogLevel::Debug) << 10;
	// Attempt to use primary culture of country.
	if (!cultureSet)
	{
		if (!tagCountry.second->getPrimaryCulture().empty())
			details.culture = tagCountry.second->getPrimaryCulture();
		else
			Log(LogLevel::Warning) << "EU4 " << tagCountry.first << " has no culture set! Defaulting.";
	}
	if (provID == 1115)
		Log(LogLevel::Debug) << 11;

	// trade goods are retained.
	details.fort = false; // dropping all forts, we'll redistribute later.
	details.inHre = srcProvince->isInHRE() || srcProvince->getClay()->getCounty()->second->isDeJureHRE();
	// base tax, production and manpower will be adjusted later.
	// not touching extra_cost until we know what it does.
	// not touching centers of trade
	if (provID == 1115)
		Log(LogLevel::Debug) << 12;

	details.cores.clear();
	details.cores.insert(tagCountry.first); // Only owner for now, dejures come later.
	details.claims.clear();						 // dejures come later.

	details.estate.clear(); // setting later.

	details.localAutonomy = 0; // let the game handle this.
	// not touching native_size/ferocity/hostileness.
	// not touching existing permanent modifiers. These mostly relate to new world anyway. Wonders do need to be added.
	if (provID == 1115)
		Log(LogLevel::Debug) << 13;

	details.shipyard = false; // we'll distribute these later.
	// not touching province_triggered_modifiers. Rome is rome.
	details.revoltRisk = 0;				 // we can adjust this later.
	details.unrest = 0;					 // ditto
	details.nationalism = 0;			 // later, if ever.
	details.seatInParliament = false; // no.
	details.jainsBurghers = false;	 // nope.
	details.rajputsNobles = false;	 // nono.
	details.brahminsChurch = false;	 // Still no.
	details.vaisyasBurghers = false;	 // No.
	if (provID == 1115)
		Log(LogLevel::Debug) << "out";
}

void EU4::Province::sterilize()
{
	details.owner.clear();
	details.controller.clear();
	details.cores.clear();
	details.claims.clear();
	details.permanentClaims.clear();
	details.inHre = false;
	details.fort = false;
	details.estate.clear();
	details.discoveredBy.clear();
	details.shipyard = false;
	details.seatInParliament = false;
	details.jainsBurghers = false;
	details.rajputsNobles = false;
	details.brahminsChurch = false;
	details.vaisyasBurghers = false;
	tagCountry = std::pair(std::string(), nullptr);
	details.isCity = false;
	// not clearing province modifiers! We're leaving great wonders right there.
	// not touching dev, CoTs and similar!
}