#include "EU4Province.h"
#include "../../CK3World/Titles/Title.h"
#include "../../Mappers/CultureMapper/CultureMapper.h"
#include "../../Mappers/ReligionMapper/ReligionMapper.h"

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