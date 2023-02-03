#include "../../CK3toEU4/Source/Mappers/DynamicIdeasMapper/DynamicIdeasMapper.h"
#include "include/gmock/gmock-matchers.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_DynamicIdeasRuleTests, nullInputIsValid)
{
	std::stringstream input;
	input << "__heritage_ with { 0 } Precedence\n";

	std::stringstream result;
	result << mappers::DynamicIdeasRule({{}}, {{}}, "", std::nullopt);

	EXPECT_EQ(input.str(), result.str());
}

TEST(Mappers_DynamicIdeasRuleTests, uniqueIdentfiersAreCrafted)
{
	std::stringstream input;
	input << "tradition_seafaring__ethos_communal_heritage_red with { 0 1 } Precedence\n";
	input << "ethos_bellicose__heritage_blue with { 0 } Precedence\n";
	input << "tradition_seafaring__heritage_red with { 0 } Precedence\n";
	input << "tradition_seafaring__ethos_communal with { 1 } Precedence\n";
	input << "tradition_seafaring__ethos_courtly with { 1 } Precedence\n";

	const auto& theMapper = mappers::DynamicIdeasMapper(mappers::LocalizationMapper());
	std::stringstream result;
	for (const auto& rule: theMapper.getRules())
	{
		result << rule;
	}
	EXPECT_EQ(input.str(), result.str());
}

TEST(Mappers_DynamicIdeasRuleTests, manualIdeaNamesStick)
{
	std::stringstream input;
	input << "paradox_idea with { 0 1 } Precedence\n";

	std::stringstream result;
	result << mappers::DynamicIdeasRule({{},
														 {
															  mappers::RULE_TYPE::ETHOS_TRIGGER,
														 }},
		 {{}},
		 "tradition_x",
		 "paradox_idea");
	EXPECT_EQ(input.str(), result.str());
}