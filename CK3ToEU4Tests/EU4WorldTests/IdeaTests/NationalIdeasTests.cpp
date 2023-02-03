#include "../../CK3toEU4/Source/EU4World/Ideas/NationalIdeas.h"
#include "include/gmock/gmock-matchers.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(EU4World_NationalIdeasTests, defualtsFillIn)
{
	std::stringstream Input;
	Input << "culture_template = akan\n";
	Input << "name = \"Akan\"\n";
	Input << "heritage = heritage_orange\n";
	Input << "ethos = ethos_bellicose\n";
	Input << "traditions = {tradition_seafaring tradition_hussar tradition_concubines tradition_astute_diplomats tradition_welcoming tradition_xenophilic}\n";

	auto culture = std::make_shared<CK3::Culture>(Input, 42);
	auto locs = mappers::LocalizationMapper();
	const EU4::NationalIdeas idea(culture, mappers::DynamicIdeasMapper(locs));

	EXPECT_EQ(idea.getTraditionIdeas().front(), "tradition_seafaring");
	EXPECT_EQ(idea.getTraditionEffects().front().front().modifier, "naval_forcelimit_modifier");
	EXPECT_EQ(idea.getTraditionEffects().front().front().value, "0.1");

	EXPECT_NE(std::find(idea.getTraditionIdeas().begin(), idea.getTraditionIdeas().end(), "increased_manpower"), idea.getTraditionIdeas().end());
	EXPECT_NE(std::find(idea.getTraditionIdeas().begin(), idea.getTraditionIdeas().end(), "increase_discipline"), idea.getTraditionIdeas().end());
	EXPECT_EQ(idea.getTraditionIdeas().size(), 8);
}

TEST(EU4World_NationalIdeasTests, ethosChangedByRules)
{
	std::stringstream Input;
	Input << "culture_template = akan\n";
	Input << "name = \"Akan\"\n";
	Input << "heritage = heritage_blue\n";
	Input << "ethos = ethos_bellicose\n";
	Input << "traditions = { tradition_seafaring }\n";

	auto culture = std::make_shared<CK3::Culture>(Input, 42);
	auto locs = mappers::LocalizationMapper();
	const EU4::NationalIdeas idea(culture, mappers::DynamicIdeasMapper(locs));

	EXPECT_EQ(idea.getEthosEffects().front().modifier, "modifier_6");
	EXPECT_EQ(idea.getEthosEffects().front().value, "0.7");
}

TEST(EU4World_NationalIdeasTests, moreSpecificRulesPreferred)
{
	std::stringstream Input;
	Input << "culture_template = akan\n";
	Input << "name = \"Akan\"\n";
	Input << "heritage = heritage_red\n";
	Input << "ethos = ethos_communal\n";
	Input << "traditions = { tradition_seafaring }\n";

	auto culture = std::make_shared<CK3::Culture>(Input, 42);
	auto locs = mappers::LocalizationMapper();
	const EU4::NationalIdeas idea(culture, mappers::DynamicIdeasMapper(locs));

	EXPECT_EQ(idea.getEthosEffects().front().modifier, "modifier_0");
	EXPECT_EQ(idea.getEthosEffects().front().value, "-0.25");
	EXPECT_EQ(idea.getEthosEffects().back().modifier, "modifier_1");
	EXPECT_EQ(idea.getEthosEffects().back().value, "0.03");

	EXPECT_EQ(idea.getTraditionIdeas().front(), "tradition_seafaring__ethos_communal_heritage_red");
	EXPECT_EQ(idea.getTraditionEffects().front().front().modifier, "prestige");
	EXPECT_EQ(idea.getTraditionEffects().front().front().value, "1");
}

TEST(EU4World_NationalIdeasTests, lowerPrecedenceDoesNotReplaceHigher)
{
	std::stringstream Input;
	Input << "culture_template = akan\n";
	Input << "name = \"Akan\"\n";
	Input << "heritage = heritage_red\n";
	Input << "ethos = ethos_bellicose\n";
	Input << "traditions = { tradition_seafaring }\n";

	auto culture = std::make_shared<CK3::Culture>(Input, 42);
	auto locs = mappers::LocalizationMapper();
	const EU4::NationalIdeas idea(culture, mappers::DynamicIdeasMapper(locs));

	EXPECT_EQ(idea.getTraditionIdeas().front(), "tradition_seafaring__heritage_red");
	EXPECT_EQ(idea.getTraditionEffects().front().front().modifier, "may_explore");
	EXPECT_EQ(idea.getTraditionEffects().front().front().value, "yes");
}