#include "../../CK3ToEU4/Source/Mappers/HeritageMapper/HeritageMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::ElementsAre;

TEST(Mappers_HeritageMappingTests, mappingDefaultsToBlank)
{
	std::stringstream input;
	const mappers::HeritageMapping mapping(input);

	EXPECT_TRUE(mapping.getCK3Heritages().empty());
	EXPECT_TRUE(mapping.getEU4CultureGroup().empty());
}

TEST(Mappers_HeritageMappingTests, mappingCanBeLoaded)
{
	std::stringstream input;
	input << R"(eu4 = "test" ck3 = "input1" ck3 = "input2" )";
	const mappers::HeritageMapping mapping(input);

	EXPECT_EQ("test", mapping.getEU4CultureGroup());
	EXPECT_THAT(mapping.getCK3Heritages(), ElementsAre("input1", "input2"));
}
