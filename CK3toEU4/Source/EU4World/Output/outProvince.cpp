#include "outProvince.h"

std::ostream& EU4::operator<<(std::ostream& output, const Province& province)
{
	if (!province.details.owner.empty())
	{
		output << "owner = " << province.details.owner << "\n";
	}
	if (!province.details.controller.empty())
		output << "controller = " << province.details.controller << "\n";
	if (!province.details.capital.empty())
		output << "capital = \"" << province.details.capital << "\"\n";
	if (province.details.isCity)
		output << "is_city = yes\n";
	if (!province.details.culture.empty())
		output << "culture = " << province.details.culture << "\n";
	if (!province.details.religion.empty())
		output << "religion = " << province.details.religion << "\n";
	if (!province.details.tradeGoods.empty())
		output << "trade_goods = " << province.details.tradeGoods << "\n";
	if (province.details.fort)
		output << "fort_15th = yes\n";
	if (province.details.inHre)
		output << "hre = yes\n";
	if (province.details.shipyard)
		output << "shipyard = yes\n";
	if (province.details.seatInParliament)
		output << "seat_in_parliament = yes\n";
	if (province.details.jainsBurghers)
		output << "add_jains_or_burghers_effect = yes\n";
	if (province.details.rajputsNobles)
		output << "add_rajputs_or_marathas_or_nobles_effect = yes\n";
	if (province.details.brahminsChurch)
		output << "add_brahmins_or_church_effect = yes\n";
	if (province.details.vaisyasBurghers)
		output << "add_vaisyas_or_burghers_effect = yes\n";
	if (province.details.baseTax)
		output << "base_tax = " << province.details.baseTax << "\n";
	if (province.details.baseProduction)
		output << "base_production = " << province.details.baseProduction << "\n";
	if (province.details.baseManpower)
		output << "base_manpower = " << province.details.baseManpower << "\n";
	if (province.details.extraCost)
		output << "extra_cost = " << province.details.extraCost << "\n";
	if (province.details.centerOfTrade)
		output << "center_of_trade = " << province.details.centerOfTrade << "\n";
	if (!province.details.cores.empty())
		for (const auto& core: province.details.cores)
			output << "add_core = " << core << "\n";
	if (!province.details.claims.empty())
		for (const auto& claim: province.details.claims)
			output << "add_claim = " << claim << "\n";
	if (!province.details.permanentClaims.empty())
		for (const auto& claim: province.details.permanentClaims)
			output << "add_permanent_claim = " << claim << "\n";
	if (!province.details.discoveredBy.empty())
		for (const auto& disc: province.details.discoveredBy)
			output << "discovered_by = " << disc << "\n";
	if (province.details.nativeSize)
		output << "native_size = " << province.details.nativeSize << "\n";
	if (province.details.nativeFerocity)
		output << "native_ferocity = " << province.details.nativeFerocity << "\n";
	if (province.details.nativeHostileness)
		output << "native_hostileness = " << province.details.nativeHostileness << "\n";
	if (!province.details.provinceModifiers.empty())
		for (const auto& modifier: province.details.provinceModifiers)
		{
			output << "add_permanent_province_modifier = {\n";
			output << "\tname = " << modifier.name << "\n";
			output << "\tduration = " << modifier.duration << "\n";
			output << "}\n";
		}
	if (!province.details.estate.empty())
		output << "estate = " << province.details.estate << "\n";
	if (!province.details.latentGoods.empty())
	{
		output << "latent_trade_goods = {\n";
		for (const auto& good: province.details.latentGoods)
		{
			output << "\t" << good << "\n";
		}
		output << "}\n";
	}
	if (!province.details.provinceTriggeredModifiers.empty())
		for (const auto& modifier: province.details.provinceTriggeredModifiers)
			output << "add_province_triggered_modifier = " << modifier << "\n";
	if (province.details.revoltRisk)
		output << "revolt_risk = " << province.details.revoltRisk << "\n";
	if (province.details.unrest)
		output << "unrest = " << province.details.unrest << "\n";
	if (province.details.nationalism)
		output << "add_nationalism = " << province.details.nationalism << "\n";
	if (!province.details.datedInfo.empty() && !province.getSourceProvince())
		output << province.details.datedInfo;
	return output;
}
