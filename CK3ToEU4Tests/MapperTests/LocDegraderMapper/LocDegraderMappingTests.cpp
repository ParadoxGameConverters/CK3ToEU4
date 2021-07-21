#include "../../CK3toEU4/Source/Mappers/LocDegraderMapper/LocDegraderMapping.h"
#include "gtest/gtest.h"

TEST(Mappers_LocDegraderMappingTests, replacementPairDefaultsToBlank)
{
	std::stringstream input;
	const mappers::LocDegraderMapping mapping(input);
	const auto& replacementMapping = mapping.getReplacementMapping();

	EXPECT_TRUE(replacementMapping.first.empty());
	EXPECT_TRUE(replacementMapping.second.empty());
}

TEST(Mappers_LocDegraderMappingTests, replacementPairCanBeLoaded)
{
	std::stringstream input;
	input << R"(utf8 = "a" win1252 = "b")";
	const mappers::LocDegraderMapping mapping(input);
	const auto& replacementMapping = mapping.getReplacementMapping();

	EXPECT_EQ("a", replacementMapping.first);
	EXPECT_EQ("b", replacementMapping.second);
}
