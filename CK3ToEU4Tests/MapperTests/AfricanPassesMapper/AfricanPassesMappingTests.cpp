#include "../../CK3toEU4/Source/Mappers/AfricanPassesMapper/AfricanPassesMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::ElementsAre;

TEST(Mappers_AfricanPassesMappingTests, passDefaultsToBlank)
{
	std::stringstream input;
	const mappers::AfricanPassesMapping mapping(input);

	EXPECT_TRUE(mapping.getName().empty());
	EXPECT_TRUE(mapping.getEndA().empty());
	EXPECT_TRUE(mapping.getEndB().empty());
	EXPECT_TRUE(mapping.getSterilizeProvinces().empty());
}

TEST(Mappers_AfricanPassesMappingTests, passCanBeLoaded)
{
	std::stringstream input;
	input << R"(name = "test" endA = {1 2} endB = {3 4} sterilize = {5 6})";
	const mappers::AfricanPassesMapping mapping(input);

	EXPECT_EQ("test", mapping.getName());
	EXPECT_THAT(mapping.getEndA(), ElementsAre(1, 2));
	EXPECT_THAT(mapping.getEndB(), ElementsAre(3, 4));
	EXPECT_THAT(mapping.getSterilizeProvinces(), ElementsAre(5, 6));
}
