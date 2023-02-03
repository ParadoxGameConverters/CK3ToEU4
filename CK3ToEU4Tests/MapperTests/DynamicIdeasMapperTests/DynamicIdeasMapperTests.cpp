#include "../../CK3toEU4/Source/Mappers/DynamicIdeasMapper/DynamicIdeasMapper.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace
{
auto locs = mappers::LocalizationMapper();
}

TEST(Mappers_DynamicIdeasMapperTests, defaultsLoadIn)
{
	const auto& theMapper = mappers::DynamicIdeasMapper(locs);

	EXPECT_THAT(theMapper.getDefaults(),
		 testing::ElementsAre("increase_discipline",
			  "increased_manpower",
			  "increase_taxation",
			  "cheaper_mercenaries",
			  "increased_trade_power",
			  "increased_production_income",
			  "placeholder_tradition"));
}

TEST(Mappers_DynamicIdeasMapperTests, ethosLinksMakeMaps)
{
	const auto& theMapper = mappers::DynamicIdeasMapper(locs);

	std::vector<std::pair<std::string, std::vector<mappers::EffectPair>>> elements;
	elements.push_back(std::make_pair("ethos_bellicose", std::vector<mappers::EffectPair>{{"modifier_4", "0.01"}, {"modifier_5", "4"}}));
	elements.push_back(std::make_pair("ethos_communal", std::vector<mappers::EffectPair>{{"modifier_0", "-0.25"}, {"modifier_1", "0.03"}}));
	elements.push_back(std::make_pair("ethos_courtly", std::vector<mappers::EffectPair>{{"modifier_2", "1"}, {"modifier_3", "0.2"}}));

	EXPECT_THAT(theMapper.getEthosMap(), testing::ElementsAreArray(elements));
}

TEST(Mappers_DynamicIdeasMapperTests, traditionLinksMakeMaps)
{
	const auto& theMapper = mappers::DynamicIdeasMapper(locs);

	std::vector<std::pair<std::string, std::vector<mappers::EffectPair>>> elements;
	elements.push_back(std::make_pair("cheaper_mercenaries", std::vector<mappers::EffectPair>{{"merc_maintenance_modifier", "-0.10"}}));
	elements.push_back(std::make_pair("increase_discipline", std::vector<mappers::EffectPair>{{"discipline", "0.025"}}));
	elements.push_back(std::make_pair("increase_taxation", std::vector<mappers::EffectPair>{{"global_tax_modifier", "0.05"}}));
	elements.push_back(std::make_pair("increased_manpower", std::vector<mappers::EffectPair>{{"global_manpower_modifier", "0.05"}}));
	elements.push_back(std::make_pair("increased_production_income", std::vector<mappers::EffectPair>{{"production_efficiency", "0.05"}}));
	elements.push_back(std::make_pair("increased_trade_power", std::vector<mappers::EffectPair>{{"global_trade_power", "0.05"}}));
	elements.push_back(std::make_pair("placeholder_tradition", std::vector<mappers::EffectPair>{{"global_tax_modifier", "0.10"}}));
	elements.push_back(std::make_pair("tradition_african_tolerance", std::vector<mappers::EffectPair>{{"religious_unity", "0.5"}}));
	elements.push_back(std::make_pair("tradition_astute_diplomats", std::vector<mappers::EffectPair>{{"diplomats", "1"}}));
	elements.push_back(std::make_pair("tradition_concubines", std::vector<mappers::EffectPair>{{"heir_chance", "0.5"}}));
	elements.push_back(std::make_pair("tradition_hereditary_bards", std::vector<mappers::EffectPair>{{"free_dip_policy", "1"}}));
	elements.push_back(std::make_pair("tradition_hussar", std::vector<mappers::EffectPair>{{"modifier_8", "0.05"}, {"modifier_9", "0.20"}}));
	elements.push_back(std::make_pair("tradition_seafaring", std::vector<mappers::EffectPair>{{"naval_forcelimit_modifier", "0.1"}}));
	elements.push_back(std::make_pair("tradition_seafaring__ethos_communal", std::vector<mappers::EffectPair>{{"galley_power", "0.1"}}));
	elements.push_back(std::make_pair("tradition_seafaring__ethos_communal_heritage_red", std::vector<mappers::EffectPair>{{"prestige", "1"}}));
	elements.push_back(std::make_pair("tradition_seafaring__ethos_courtly", std::vector<mappers::EffectPair>{{"heavy_ship_power", "0.1"}}));
	elements.push_back(std::make_pair("tradition_seafaring__heritage_red", std::vector<mappers::EffectPair>{{"may_explore", "yes"}}));
	elements.push_back(std::make_pair("tradition_welcoming", std::vector<mappers::EffectPair>{{"diplomatic_upkeep", "1"}}));
	elements.push_back(std::make_pair("tradition_xenophilic", std::vector<mappers::EffectPair>{{"num_accepted_cultures", "1"}}));

	EXPECT_THAT(theMapper.getTraditionMap(), testing::ElementsAreArray(elements));
}

TEST(Mappers_DynamicIdeasMapperTests, ruleLinksMakeRules)
{
	const auto& theMapper = mappers::DynamicIdeasMapper(locs);

	std::set<mappers::DynamicIdeasRule> elements;

	elements.emplace(mappers::DynamicIdeasRule(
		 std::vector<mappers::RulePair>{{mappers::RULE_TYPE::ETHOS_TRIGGER, "ethos_communal"}, {mappers::RULE_TYPE::HERITAGE, "heritage_red"}},
		 std::vector<mappers::EffectPair>{{"prestige", "1"}},
		 "tradition_seafaring",
		 std::nullopt));
	elements.emplace(mappers::DynamicIdeasRule(std::vector<mappers::RulePair>{{mappers::RULE_TYPE::HERITAGE, "heritage_blue"}},
		 std::vector<mappers::EffectPair>{{"modifier_6", "0.7"}},
		 "ethos_bellicose",
		 std::nullopt));
	elements.emplace(mappers::DynamicIdeasRule(std::vector<mappers::RulePair>{{mappers::RULE_TYPE::HERITAGE, "heritage_red"}},
		 std::vector<mappers::EffectPair>{{"may_explore", "yes"}},
		 "tradition_seafaring",
		 std::nullopt));
	elements.emplace(mappers::DynamicIdeasRule(std::vector<mappers::RulePair>{{mappers::RULE_TYPE::ETHOS_TRIGGER, "ethos_communal"}},
		 std::vector<mappers::EffectPair>{{"galley_power", "0.1"}},
		 "tradition_seafaring",
		 std::nullopt));
	elements.emplace(mappers::DynamicIdeasRule(std::vector<mappers::RulePair>{{mappers::RULE_TYPE::ETHOS_TRIGGER, "ethos_courtly"}},
		 std::vector<mappers::EffectPair>{{"heavy_ship_power", "0.1"}},
		 "tradition_seafaring",
		 std::nullopt));

	EXPECT_THAT(theMapper.getRules(), testing::ElementsAreArray(elements));
}