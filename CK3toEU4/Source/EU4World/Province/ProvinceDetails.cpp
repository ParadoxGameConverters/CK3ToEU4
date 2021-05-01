#include "ProvinceDetails.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::ProvinceDetails::ProvinceDetails(const std::string& filePath)
{
	registerKeys();

	if (commonItems::DoesFileExist(filePath))
	{
		parseFile(filePath);
	}
	clearRegisteredKeywords();
}

void EU4::ProvinceDetails::updateWith(const std::string& filePath)
{
	registerKeys();
	if (commonItems::DoesFileExist(filePath))
	{
		parseFile(filePath);
	}
	clearRegisteredKeywords();
}

EU4::ProvinceDetails::ProvinceDetails(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ProvinceDetails::registerKeys()
{
	registerKeyword("owner", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString ownerStr(theStream);
		owner = ownerStr.getString();
	});
	registerKeyword("controller", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString contStr(theStream);
		controller = contStr.getString();
	});
	registerKeyword("capital", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString capStr(theStream);
		capital = capStr.getString();
	});
	registerKeyword("is_city", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString cityStr(theStream);
		isCity = cityStr.getString() == "yes";
	});
	registerKeyword("culture", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString cultureStr(theStream);
		culture = cultureStr.getString();
	});
	registerKeyword("religion", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString religStr(theStream);
		religion = religStr.getString();
	});
	registerKeyword("trade_goods", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString goodsStr(theStream);
		tradeGoods = goodsStr.getString();
	});
	registerKeyword("fort_15th", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString fortStr(theStream);
		fort = fortStr.getString() == "yes";
	});
	registerKeyword("hre", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString hreStr(theStream);
		inHre = hreStr.getString() == "yes";
	});
	registerKeyword("base_tax", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt taxInt(theStream);
		baseTax = taxInt.getInt();
	});
	registerKeyword("base_production", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt prodInt(theStream);
		baseProduction = prodInt.getInt();
	});
	registerKeyword("base_manpower", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt manInt(theStream);
		baseManpower = manInt.getInt();
	});
	registerKeyword("extra_cost", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt costInt(theStream);
		extraCost = costInt.getInt();
	});
	registerKeyword("center_of_trade", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt cotInt(theStream);
		centerOfTrade = cotInt.getInt();
	});
	registerKeyword("add_core", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString coreStr(theStream);
		cores.insert(coreStr.getString());
	});
	registerKeyword("add_claim", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString claimStr(theStream);
		claims.insert(claimStr.getString());
	});
	registerKeyword("add_permanent_claim", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString claimStr(theStream);
		permanentClaims.insert(claimStr.getString());
	});
	registerKeyword("discovered_by", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString discoveredStr(theStream);
		discoveredBy.insert(discoveredStr.getString());
	});
	registerKeyword("add_local_autonomy", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt autoInt(theStream);
		localAutonomy = autoInt.getInt();
	});
	registerKeyword("native_size", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt sizeInt(theStream);
		nativeSize = sizeInt.getInt();
	});
	registerKeyword("native_ferocity", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt ferocityInt(theStream);
		nativeFerocity = ferocityInt.getInt();
	});
	registerKeyword("native_hostileness", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt hostInt(theStream);
		nativeHostileness = hostInt.getInt();
	});
	registerKeyword("add_permanent_province_modifier", [this](const std::string& unused, std::istream& theStream) {
		const auto newModifier = ProvinceModifier(theStream);
		provinceModifiers.emplace_back(newModifier);
	});
	registerKeyword("estate", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString estateStr(theStream);
		estate = estateStr.getString();
	});
	registerKeyword("latent_trade_goods", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::stringList theList(theStream);
		auto theGoods = theList.getStrings();
		latentGoods.insert(theGoods.begin(), theGoods.end());
	});
	registerKeyword("shipyard", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString shipStr(theStream);
		shipyard = shipStr.getString() == "yes";
	});
	registerKeyword("add_province_triggered_modifier", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString modStr(theStream);
		provinceTriggeredModifiers.insert(modStr.getString());
	});
	registerKeyword("revolt_risk", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt revoltInt(theStream);
		revoltRisk = revoltInt.getInt();
	});
	registerKeyword("unrest", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt unrestInt(theStream);
		unrest = unrestInt.getInt();
	});
	registerKeyword("add_nationalism", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleInt nationalismInt(theStream);
		nationalism = nationalismInt.getInt();
	});
	registerKeyword("seat_in_parliament", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString seatStr(theStream);
		seatInParliament = seatStr.getString() == "yes";
	});
	registerKeyword("add_jains_or_burghers_effect", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString effectStr(theStream);
		jainsBurghers = effectStr.getString() == "yes";
	});
	registerKeyword("add_rajputs_or_marathas_or_nobles_effect", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString effectStr(theStream);
		rajputsNobles = effectStr.getString() == "yes";
	});
	registerKeyword("add_brahmins_or_church_effect", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString effectStr(theStream);
		brahminsChurch = effectStr.getString() == "yes";
	});
	registerKeyword("add_vaisyas_or_burghers_effect", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString effectStr(theStream);
		vaisyasBurghers = effectStr.getString() == "yes";
	});
	// These two hard overrides are necessary for Palembang and similar countries outside CK scope that are assigned ownership in a 1444 history block instead directly.
	// Without us replicating the 1441/1444 block they'd remain uncolonized in any conversion.
	registerKeyword("1441.1.1", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString effectStr(theStream);
		datedInfo = effectStr.getString();
	});
	registerKeyword("1444.1.1", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString effectStr(theStream);
		datedInfo = effectStr.getString();
	});	
	registerRegex(R"(\d+.\d+.\d+)", commonItems::ignoreItem);
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
