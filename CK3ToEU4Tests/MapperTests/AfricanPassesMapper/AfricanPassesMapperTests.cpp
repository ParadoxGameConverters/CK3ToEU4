#include "../../CK3ToEU4/Source/Mappers/AfricanPassesMapper/AfricanPassesMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_AfricanPasseseMapperTests, passesDefaultToBlank)
{
	std::stringstream input;
	const mappers::AfricanPassesMapper mapper(input);

	EXPECT_TRUE(mapper.getPasses().empty());
}

TEST(Mappers_AfricanPasseseMapperTests, passesCanBeLoaded)
{
	std::stringstream input;
	input << R"(pass = { name = "test" endA = {1 2} endB = {3 4} sterilize = {5 6} })";
	input << R"(pass = { name = "test2" endA = {1 2} endB = {3 4} sterilize = {5 6} })";
	const mappers::AfricanPassesMapper mapper(input);

	EXPECT_EQ(2, mapper.getPasses().size());
	EXPECT_EQ("test", mapper.getPasses()[0].getName());
	EXPECT_EQ("test2", mapper.getPasses()[1].getName());
}
