#include "../../CK3toEU4/Source/Mappers/HeritageMapper/HeritageMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_HeritageMapperTests, heritageCanBePinged)
{
	std::stringstream input;
	input << R"(link = { eu4 = "test1" ck3 = input1 ck3 = input2 })";
	input << R"(link = { eu4 = "test2" ck3 = input3 ck3 = input4 })";
	const mappers::HeritageMapper mapper(input);

	EXPECT_EQ("test1", mapper.getCultureGroupForHeritage("input2"));
	EXPECT_EQ("test2", mapper.getCultureGroupForHeritage("input3"));
}

TEST(Mappers_HeritageMapperTests, missingLinkReturnsNullopt)
{
	std::stringstream input;
	input << R"(link = { eu4 = "test1" ck3 = input1 ck3 = input2 })";
	input << R"(link = { eu4 = "test2" ck3 = input3 ck3 = input4 })";
	const mappers::HeritageMapper mapper(input);

	EXPECT_EQ(std::nullopt, mapper.getCultureGroupForHeritage("input7"));
}
