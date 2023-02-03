#include "../../CK3toEU4/Source/Mappers/DynamicIdeasMapper/DynamicIdeasLink.h"
#include "include/gmock/gmock-matchers.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_DynamicIdeasLinkTests, nullInputIsValid)
{
	std::stringstream input;
	input << "ck3 = {}";
	input << "eu4 = {}";

	const auto& link = mappers::DynamicIdeasLink(input);
	EXPECT_EQ(link.getTradition(), std::nullopt);
	EXPECT_EQ(link.getEthos(), std::nullopt);
	EXPECT_EQ(link.getDefault(), std::nullopt);
	EXPECT_EQ(link.getIdeaName(), std::nullopt);
	EXPECT_TRUE(link.getEffects().empty());
	EXPECT_TRUE(link.getRules().empty());
}

TEST(Mappers_DynamicIdeasLinkTests, traditionIsReadIn)
{
	std::stringstream input;
	input << "ck3 = { tradition = x }";
	input << "eu4 = { mod0 = 0.1 mod1 = 1 }";

	const auto& mods = std::vector<mappers::EffectPair>{{"mod0", "0.1"}, {"mod1", "1"}};

	const auto& link = mappers::DynamicIdeasLink(input);
	EXPECT_EQ(link.getTradition(), "x");
	EXPECT_EQ(link.getEthos(), std::nullopt);
	EXPECT_EQ(link.getDefault(), std::nullopt);
	EXPECT_EQ(link.getIdeaName(), std::nullopt);
	EXPECT_THAT(link.getEffects(), testing::ElementsAreArray(mods));
	EXPECT_TRUE(link.getRules().empty());
}

TEST(Mappers_DynamicIdeasLinkTests, ethosIsReadIn)
{
	std::stringstream input;
	input << "ck3 = { ethos = e }";
	input << "eu4 = { mod1 = 1 }";

	const auto& mods = std::vector<mappers::EffectPair>{{"mod1", "1"}};

	const auto& link = mappers::DynamicIdeasLink(input);
	EXPECT_EQ(link.getTradition(), std::nullopt);
	EXPECT_EQ(link.getEthos(), "e");
	EXPECT_EQ(link.getDefault(), std::nullopt);
	EXPECT_EQ(link.getIdeaName(), std::nullopt);
	EXPECT_THAT(link.getEffects(), testing::ElementsAreArray(mods));
	EXPECT_TRUE(link.getRules().empty());
}

TEST(Mappers_DynamicIdeasLinkTests, defaultIsReadIn)
{
	std::stringstream input;
	input << "ck3 = { default = d }";
	input << "eu4 = { mod0 = 0.1 }";

	const auto& mods = std::vector<mappers::EffectPair>{{"mod0", "0.1"}};

	const auto& link = mappers::DynamicIdeasLink(input);
	EXPECT_EQ(link.getTradition(), std::nullopt);
	EXPECT_EQ(link.getEthos(), std::nullopt);
	EXPECT_EQ(link.getDefault(), "d");
	EXPECT_EQ(link.getIdeaName(), std::nullopt);
	EXPECT_THAT(link.getEffects(), testing::ElementsAreArray(mods));
	EXPECT_TRUE(link.getRules().empty());
}

TEST(Mappers_DynamicIdeasLinkTests, ruleIsReadIn)
{
	std::stringstream input;
	input << "ck3 = { tradition = x }";
	input << "eu4 = { mod2 = -0.1 }";
	input << "rule = { heritage = val0 ethos_rule = val1 }";

	const auto& rules = std::vector<mappers::RulePair>{{mappers::RULE_TYPE::ETHOS_TRIGGER, "val1"}, {mappers::RULE_TYPE::HERITAGE, "val0"}};
	const auto& mods = std::vector<mappers::EffectPair>{{"mod2", "-0.1"}};

	const auto& link = mappers::DynamicIdeasLink(input);
	EXPECT_EQ(link.getTradition(), "x");
	EXPECT_EQ(link.getEthos(), std::nullopt);
	EXPECT_EQ(link.getDefault(), std::nullopt);
	EXPECT_EQ(link.getIdeaName(), std::nullopt);
	EXPECT_THAT(link.getEffects(), testing::ElementsAreArray(mods));
	EXPECT_THAT(link.getRules(), testing::ElementsAreArray(rules));
}

TEST(Mappers_DynamicIdeasLinkTests, soloIdeaNameIsReadIn)
{
	std::stringstream input;
	input << "ck3 = { tradition = x }";
	input << "eu4 = { mod2 = -0.1 }";
	input << "rule = { heritage = val0 }";
	input << "idea_name = n";

	const auto& rules = std::vector<mappers::RulePair>{{mappers::RULE_TYPE::HERITAGE, "val0"}};
	const auto& mods = std::vector<mappers::EffectPair>{{"mod2", "-0.1"}};

	const auto& link = mappers::DynamicIdeasLink(input);
	EXPECT_EQ(link.getTradition(), "x");
	EXPECT_EQ(link.getEthos(), std::nullopt);
	EXPECT_EQ(link.getDefault(), std::nullopt);
	EXPECT_EQ(link.getIdeaName(), "n");
	EXPECT_THAT(link.getEffects(), testing::ElementsAreArray(mods));
	EXPECT_THAT(link.getRules(), testing::ElementsAreArray(rules));
}

TEST(Mappers_DynamicIdeasLinkTests, nestedIdeaNameIsReadIn)
{
	std::stringstream input;
	input << "ck3 = { tradition = x }";
	input << "eu4 = { mod2 = -0.1 }";
	input << "rule = { heritage = val0 idea_name = n }";

	const auto& rules = std::vector<mappers::RulePair>{{mappers::RULE_TYPE::HERITAGE, "val0"}};
	const auto& mods = std::vector<mappers::EffectPair>{{"mod2", "-0.1"}};

	const auto& link = mappers::DynamicIdeasLink(input);
	EXPECT_EQ(link.getTradition(), "x");
	EXPECT_EQ(link.getEthos(), std::nullopt);
	EXPECT_EQ(link.getDefault(), std::nullopt);
	EXPECT_EQ(link.getIdeaName(), "n");
	EXPECT_THAT(link.getEffects(), testing::ElementsAreArray(mods));
	EXPECT_THAT(link.getRules(), testing::ElementsAreArray(rules));
}