#include "../../CK3toEU4/Source/CK3World/Titles/DynamicTemplate.h"
#include "gtest/gtest.h"

TEST(CK3World_DynamicTemplateTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const CK3::DynamicTemplate dynamicTemplate(input);

	ASSERT_TRUE(dynamicTemplate.getDynamicTitleKey().empty());
	ASSERT_TRUE(dynamicTemplate.getDynamicTitleRank().empty());
}

TEST(CK3World_DynamicTemplateTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "key=\"x_x_3201\"\n";
	input << "tier = empire\n";
	const CK3::DynamicTemplate dynamicTemplate(input);

	ASSERT_EQ("x_x_3201", dynamicTemplate.getDynamicTitleKey());
	ASSERT_EQ("empire", dynamicTemplate.getDynamicTitleRank());
}
