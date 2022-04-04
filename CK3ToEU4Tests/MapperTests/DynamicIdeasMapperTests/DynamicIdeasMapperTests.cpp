#include "../../../CK3toEU4/Source/EU4World/Ideas/NationalIdeas.h"
#include "../CK3ToEU4/Source/Mappers/DynamicIdeasMapper/DynamicIdeasMapper.h"
#include "gtest/gtest.h"
#include <CommonFunctions.h>
#include <sstream>

// TEST(Mappers_DynamicIdeasMapperTests, brokenLinkIsIgnored)

TEST(Mappers_DynamicIdeasMapperTests, linksMakeMaps)
{
	// $(ExecutableDir) is CK3ToEU4Tests/Release. Should the test file be in or be copied to there?
	const auto& theMapper = mappers::DynamicIdeasMapper("../TestFiles/configurables/tradition_ideas.txt", mappers::LocalizationMapper::LocalizationMapper());

	EXPECT_EQ(theMapper.getEthosMap().size(), 3);
	EXPECT_EQ(theMapper.getTraditionMap().size(), 19);
	EXPECT_EQ(theMapper.getDefaults().size(), 7);
	EXPECT_TRUE(theMapper.getEthosMap().contains("ethos_bellicose"));
	EXPECT_TRUE(theMapper.getTraditionMap().contains("tradition_hussar"));
	EXPECT_TRUE(theMapper.getTraditionMap().contains("increase_discipline"));
	EXPECT_TRUE(theMapper.getDefaults().front().compare("increase_discipline") == 0);
	EXPECT_EQ(theMapper.getRules().size(), 5);
}

TEST(Mappers_DynamicIdeasMapperTests, defualtsFillIn)
{
	// Base mappings apply when no rules are true, and defualts used as filler
	std::stringstream Input;
	Input << "culture_template = akan\n";
	Input << "name = \"Akan\"\n";
	Input << "heritage = heritage_orange\n";
	Input << "ethos = ethos_bellicose\n";
	Input << "traditions = {tradition_seafaring tradition_hussar tradition_concubines tradition_astute_diplomats tradition_welcoming tradition_xenophilic}\n";

	auto culture = std::make_shared<CK3::Culture>(Input, 42);
	const EU4::NationalIdeas idea(culture,
		 mappers::DynamicIdeasMapper("../TestFiles/configurables/tradition_ideas.txt", mappers::LocalizationMapper::LocalizationMapper()));

	EXPECT_EQ(idea.getTraditionIdeas().front(), "tradition_seafaring");
	EXPECT_EQ(idea.getTraditionEffects().front().front().modifier, "naval_forcelimit_modifier");
	EXPECT_EQ(idea.getTraditionEffects().front().front().value, "0.1");

	EXPECT_NE(std::find(idea.getTraditionIdeas().begin(), idea.getTraditionIdeas().end(), "increased_manpower"), idea.getTraditionIdeas().end());
	EXPECT_NE(std::find(idea.getTraditionIdeas().begin(), idea.getTraditionIdeas().end(), "increase_discipline"), idea.getTraditionIdeas().end());
	EXPECT_EQ(idea.getTraditionIdeas().size(), 8);
}

TEST(Mappers_DynamicIdeasMapperTests, ethosRulesApply)
{
	// Ethos rules work
	std::stringstream Input;
	Input << "culture_template = akan\n";
	Input << "name = \"Akan\"\n";
	Input << "heritage = heritage_blue\n";
	Input << "ethos = ethos_bellicose\n";
	Input << "traditions = { tradition_seafaring }\n";

	auto culture = std::make_shared<CK3::Culture>(Input, 42);
	const EU4::NationalIdeas idea(culture,
		 mappers::DynamicIdeasMapper("../TestFiles/configurables/tradition_ideas.txt", mappers::LocalizationMapper::LocalizationMapper()));

	EXPECT_EQ(idea.getEthosEffects().front().modifier, "possible_condottieri");
	EXPECT_EQ(idea.getEthosEffects().front().value, "1");
	EXPECT_EQ(idea.getEthosEffects().back().modifier, "loot_amount");
	EXPECT_EQ(idea.getEthosEffects().back().value, "0.10");
}

TEST(Mappers_DynamicIdeasMapperTests, moreSpecificOverLessSpecific)
{
	// More specific rules prefered over less & normal ethos mapping still works
	std::stringstream Input;
	Input << "culture_template = akan\n";
	Input << "name = \"Akan\"\n";
	Input << "heritage = heritage_red\n";
	Input << "ethos = ethos_communal\n";
	Input << "traditions = { tradition_seafaring }\n";

	auto culture = std::make_shared<CK3::Culture>(Input, 42);
	const EU4::NationalIdeas idea(culture,
		 mappers::DynamicIdeasMapper("../TestFiles/configurables/tradition_ideas.txt", mappers::LocalizationMapper::LocalizationMapper()));

	EXPECT_EQ(idea.getEthosEffects().front().modifier, "same_culture_advisor_cost");
	EXPECT_EQ(idea.getEthosEffects().front().value, "-0.25");
	EXPECT_EQ(idea.getEthosEffects().back().modifier, "global_heretic_missionary_strength");
	EXPECT_EQ(idea.getEthosEffects().back().value, "0.03");

	EXPECT_EQ(idea.getTraditionIdeas().front(), "tradition_seafaring__ethos_communal_heritage_red");
	EXPECT_EQ(idea.getTraditionEffects().front().front().modifier, "prestige");
	EXPECT_EQ(idea.getTraditionEffects().front().front().value, "1");
}

TEST(Mappers_DynamicIdeasMapperTests, precedenceCheck)
{
	// Lower precedence rules do not replace higher ones
	std::stringstream Input;
	Input << "culture_template = akan\n";
	Input << "name = \"Akan\"\n";
	Input << "heritage = heritage_red\n";
	Input << "ethos = ethos_bellicose\n";
	Input << "traditions = { tradition_seafaring }\n";

	auto culture = std::make_shared<CK3::Culture>(Input, 42);
	const EU4::NationalIdeas idea(culture,
		 mappers::DynamicIdeasMapper("../TestFiles/configurables/tradition_ideas.txt", mappers::LocalizationMapper::LocalizationMapper()));

	EXPECT_EQ(idea.getTraditionIdeas().front(), "tradition_seafaring__heritage_red");
	EXPECT_EQ(idea.getTraditionEffects().front().front().modifier, "may_explore");
	EXPECT_EQ(idea.getTraditionEffects().front().front().value, "yes");
}