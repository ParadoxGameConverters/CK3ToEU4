#include "../../CK3toEU4/Source/Mappers/IslamOverrideMapper/IslamOverrideMapping.h"
#include "gtest/gtest.h"

TEST(Mappers_IslamOverrideMappingTests, replacementPairDefaultsToBlank)
{
	std::stringstream input;
	const mappers::IslamOverrideMapping mapping(input);
	const auto& replacementMapping = mapping.getReplacementMapping();

	EXPECT_EQ(0, replacementMapping.first);
	EXPECT_TRUE(replacementMapping.second.empty());
}

TEST(Mappers_IslamOverrideMappingTests, replacementPairCanBeLoaded)
{
	std::stringstream input;
	input << "province = 1 religion = test";
	const mappers::IslamOverrideMapping mapping(input);
	const auto& replacementMapping = mapping.getReplacementMapping();

	EXPECT_EQ(1, replacementMapping.first);
	EXPECT_EQ("test", replacementMapping.second);
}
