#include "EU4Province.h"
#include "../../CK3World/Titles/Title.h"
#include "../../Mappers/CultureMapper/CultureMapper.h"
#include "../../Mappers/ReligionMapper/ReligionMapper.h"
#include "../Country/Country.h"
#include "../../CK3World/Characters/Character.h"

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

void EU4::Province::initializeFromCK3(std::shared_ptr<CK3::Title> origProvince,
	 const mappers::CultureMapper& cultureMapper,
	 const mappers::ReligionMapper& religionMapper)
{
	srcProvince = std::move(origProvince);
	details.discoveredBy = {"eastern", "western", "muslim", "ottoman", "indian", "nomad_group"}; // hardcoding for now.
	/*
	// If we're initializing this from CK2 provinces, then having an owner or being a wasteland/sea is a given -
	// there are no uncolonized provinces in CK2.
	if (srcProvince->getTitle().first.empty())
		return;																 // wasteland.
	tagCountry = srcProvince->getTitle().second->getEU4Tag(); // linking to our holder
	details.owner = tagCountry.first;
	details.controller = tagCountry.first;

	// History section
	// Not touching Capital, that's hardcoded English name.
	details.isCity = true; // This must be true for all incoming provinces.

	// Religion first.
	auto religionSet = false;
	if (!srcProvince->getReligion().empty())
	{
		auto religionMatch = religionMapper.getEU4ReligionForCK3Religion(srcProvince->getReligion());
		if (religionMatch)
		{
			details.religion = *religionMatch;
			religionSet = true;
		}
	}
	// Attempt to use religion of ruler in THAT province.
	if (!religionSet && srcProvince->getTitle().second->getHolder().first && !srcProvince->getTitle().second->getHolder().second->getReligion().empty())
	{
		auto religionMatch = religionMapper.getEU4ReligionForCK3Religion(srcProvince->getTitle().second->getHolder().second->getReligion());
		if (religionMatch)
		{
			details.religion = *religionMatch;
			religionSet = true;
		}
	}
	// Attempt to use religion of country.
	if (!religionSet && !tagCountry.second->getReligion().empty())
	{
		details.religion = tagCountry.second->getReligion();
		religionSet = true;
	}
	if (!religionSet)
	{
		// owner has no religion, which is common for hordeland. We should use default eu4 religion.
	}
	auto cultureSet = false;
	// do we even have a base culture?
	if (!srcProvince->getCulture().empty())
	{
		auto cultureMatch = cultureMapper.cultureMatch(srcProvince->getCulture(), details.religion, provID, tagCountry.first);
		if (cultureMatch)
		{
			details.culture = *cultureMatch;
			cultureSet = true;
		}
	}
	// Attempt to use primary culture of ruler in THAT province.
	if (!cultureSet && srcProvince->getTitle().second->getHolder().first && !srcProvince->getTitle().second->getHolder().second->getCulture().empty())
	{
		auto cultureMatch = cultureMapper.cultureMatch(srcProvince->getTitle().second->getHolder().second->getCulture(), details.religion, provID, tagCountry.first);
		if (cultureMatch)
		{
			details.culture = *cultureMatch;
			cultureSet = true;
		}
	}
	// Attempt to use primary culture of country.
	if (!cultureSet && !tagCountry.second->getPrimaryCulture().empty())
	{
		details.culture = tagCountry.second->getPrimaryCulture();
		cultureSet = true;
	}
	if (!cultureSet)
	{
		// owner has no culture, which is common for hordeland. We should use default eu4 culture.
	}
	// trade goods are retained.
	details.fort = false; // dropping all forts, we'll redistribute later.
	details.inHre = srcProvince->getTitle().second->isInHRE() || srcProvince->isDeJureHRE();
	// base tax, production and manpower will be adjusted later.
	// not touching extra_cost until we know what it does.
	// not touching centers of trade

	details.cores.clear();
	details.cores.insert(tagCountry.first); // Only owner for now, dejures come later.
	details.claims.clear();						 // dejures come later.

	details.estate.clear(); // setting later.

	details.localAutonomy = 0; // let the game handle this.
	// not touching native_size/ferocity/hostileness.
	// not touching existing permanent modifiers. These mostly relate to new world anyway. Wonders do need to be added.
	if (srcProvince->getWonder().first && !srcProvince->getWonder().second->isSpent())
	{
		ProvinceModifier newModifier;
		newModifier.name = srcProvince->getWonder().second->getType();
		details.provinceModifiers.emplace_back(newModifier);
		srcProvince->getWonder().second->setSpent(); // We must spend it to avoid mapping it into multiple eu4 provinces.
	}
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
	*/
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