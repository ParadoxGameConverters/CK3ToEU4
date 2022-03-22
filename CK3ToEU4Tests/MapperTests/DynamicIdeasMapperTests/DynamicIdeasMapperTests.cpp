#include "../../../CK3toEU4/Source/EU4World/Ideas/NationalIdeas.h"
#include "../CK3ToEU4/Source/Mappers/DynamicIdeasMapper/DynamicIdeasMapper.h"
#include "gtest/gtest.h"
#include <sstream>
#include <Log.h>

// TEST(Mappers_DynamicIdeasMapperTests, brokenLinkIsIgnored)

TEST(Mappers_DynamicIdeasMapperTests, linksMakeMaps)
{
	std::stringstream input;
	input << "link = { ck3 = { ethos = ethos_communal } eu4 = { same_culture_advisor_cost = -0.25 global_heretic_missionary_strength = 0.03 } }";
	input << "link = { ck3 = { ethos = ethos_courtly } eu4 = { monthly_splendor = 1 power_projection_from_insults = 0.2 } }";
	input << "link = { ck3 = { ethos = ethos_bellicose } eu4 = { yearly_army_professionalism = 0.01 land_forcelimit = 4 } }";
	input << "link = { ck3 = { ethos = ethos_bellicose } rule = { heritage = heritage_blue } eu4 = { possible_condottieri = 1 loot_amount = 0.10 } }";
	input << "link = { ck3 = { tradition = tradition_african_tolerance } eu4 = { religious_unity = 0.5 } } ";
	input << "link = { ck3 = { tradition = tradition_concubines } eu4 = { heir_chance = 0.5 } } ";
	input << "link = { ck3 = { tradition = tradition_astute_diplomats } eu4 = { diplomats = 1 } }";
	input << "link = { ck3 = { tradition = tradition_seafaring } eu4 = { naval_forcelimit_modifier = 0.1 } }";
	input << "link = { ck3 = { tradition = tradition_welcoming } eu4 = { diplomatic_upkeep = 1 } }";
	input << "link = { ck3 = { tradition = tradition_xenophilic } eu4 = { num_accepted_cultures = 1 } }";
	input << "link = { ck3 = { tradition = tradition_hereditary_bards } eu4 = { free_dip_policy = 1 } }";
	input << "link = { ck3 = { tradition = tradition_hussar } eu4 = { special_unit_forcelimit = 0.05 cavalry_flanking = 0.20 } } ";
	input << "link = { ck3 = { default = increase_discipline } eu4 = { discipline = 0.025 } }";
	input << "link = { ck3 = { default = increased_manpower } eu4 = { global_manpower_modifier = 0.05 } }";
	input << "link = { ck3 = { default = increase_taxation } eu4 = { global_tax_modifier = 0.05 } }";
	input << "link = { ck3 = { default = cheaper_mercenaries } eu4 = { merc_maintenance_modifier = -0.10 } }";
	input << "link = { ck3 = { default = increased_trade_power } eu4 = { global_trade_power = 0.05 } }";
	input << "link = { ck3 = { default = increased_production_income } eu4 = { production_efficiency = 0.05 } }";
	input << "link = { ck3 = { default = placeholder_tradition } eu4 = { global_tax_modifier = 0.10 } }";
	input << "link = { ck3 = { tradition = tradition_seafaring } rule = { heritage = heritage_red } eu4 = { may_explore = yes } }";
	input << "link = { ck3 = { tradition = tradition_seafaring } rule = { ethos_rule = ethos_communal } eu4 = { galley_power = 0.1} }";
	input << "link = { ck3 = { tradition = tradition_seafaring } rule = { heritage = heritage_red ethos_rule = ethos_communal } eu4 = { prestige = 1 } }";
	input << "link = { ck3 = { tradition = tradition_seafaring } rule = { ethos_rule = ethos_courtly } eu4 = { heavy_ship_power = 0.1 } }";

	const mappers::DynamicIdeasMapper theMapper(input);

	EXPECT_EQ(theMapper.getEthosMap().size(), 3);
	EXPECT_EQ(theMapper.getTraditionMap().size(), 19);
	EXPECT_EQ(theMapper.getDefaults().size(), 7);
	EXPECT_TRUE(theMapper.getEthosMap().contains("ethos_bellicose"));
	EXPECT_TRUE(theMapper.getTraditionMap().contains("tradition_hussar"));
	EXPECT_TRUE(theMapper.getTraditionMap().contains("increase_discipline"));
	EXPECT_TRUE(theMapper.getDefaults().front().compare("increase_discipline") == 0);
	EXPECT_EQ(theMapper.getRules().size(), 5);

	// Ethos rules work, base mappings apply when no rules are true, and defualts used as filler
	std::stringstream cInput;
	cInput << "culture_template = akan\n";
	cInput << "name = \"Akan\"\n";
	cInput << "heritage = heritage_blue\n";
	cInput << "ethos = ethos_bellicose\n";
	cInput << "traditions = {tradition_seafaring tradition_hussar tradition_concubines tradition_astute_diplomats tradition_welcoming tradition_xenophilic}\n";

	auto culture0 = std::make_shared<CK3::Culture>(cInput, 42);
	const EU4::NationalIdeas idea0(culture0, theMapper);
	EXPECT_EQ(idea0.getEthosEffects().front().type, "possible_condottieri");
	EXPECT_EQ(idea0.getEthosEffects().front().value, "1");
	EXPECT_EQ(idea0.getEthosEffects().back().type, "loot_amount");
	EXPECT_EQ(idea0.getEthosEffects().back().value, "0.10");

	EXPECT_EQ(idea0.getTraditionIdeas().front(), "tradition_seafaring");
	EXPECT_EQ(idea0.getTraditionEffects().front().front().type, "naval_forcelimit_modifier");
	EXPECT_EQ(idea0.getTraditionEffects().front().front().value, "0.1");
	
	EXPECT_NE(std::find(idea0.getTraditionIdeas().begin(), idea0.getTraditionIdeas().end(), "increased_manpower"), idea0.getTraditionIdeas().end());
	EXPECT_NE(std::find(idea0.getTraditionIdeas().begin(), idea0.getTraditionIdeas().end(), "increase_discipline"), idea0.getTraditionIdeas().end());
	EXPECT_EQ(idea0.getTraditionIdeas().size(), 8);
	
	// More specific rules prefered over less & normal ethos mapping works
	std::stringstream dInput;
	dInput << "culture_template = akan\n";
	dInput << "name = \"Akan\"\n";
	dInput << "heritage = heritage_red\n";
	dInput << "ethos = ethos_communal\n";
	dInput << "traditions = { tradition_seafaring }\n";

	auto culture1 = std::make_shared<CK3::Culture>(dInput, 42);
	const EU4::NationalIdeas idea1(culture1, theMapper);

	EXPECT_EQ(idea1.getEthosEffects().front().type, "same_culture_advisor_cost");
	EXPECT_EQ(idea1.getEthosEffects().front().value, "-0.25");
	EXPECT_EQ(idea1.getEthosEffects().back().type, "global_heretic_missionary_strength");
	EXPECT_EQ(idea1.getEthosEffects().back().value, "0.03");

	EXPECT_EQ(idea1.getTraditionIdeas().front(), "tradition_seafaring_heritage_red_ethos_communal");
	EXPECT_EQ(idea1.getTraditionEffects().front().front().type, "prestige");
	EXPECT_EQ(idea1.getTraditionEffects().front().front().value, "1");
	
	// Lower precedence rules do not replace higher ones
	std::stringstream eInput;
	eInput << "culture_template = akan\n";
	eInput << "name = \"Akan\"\n";
	eInput << "heritage = heritage_red\n";
	eInput << "ethos = ethos_bellicose\n";
	eInput << "traditions = { tradition_seafaring }\n";

	auto culture2 = std::make_shared<CK3::Culture>(eInput, 42);
	const EU4::NationalIdeas idea2(culture2, theMapper);

	EXPECT_EQ(idea2.getTraditionIdeas().front(), "tradition_seafaring_heritage_red");
	EXPECT_EQ(idea2.getTraditionEffects().front().front().type, "may_explore");
	EXPECT_EQ(idea2.getTraditionEffects().front().front().value, "yes");
}