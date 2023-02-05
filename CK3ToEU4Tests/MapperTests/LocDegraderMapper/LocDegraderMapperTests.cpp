#include "../../CK3ToEU4/Source/Mappers/LocDegraderMapper/LocDegraderMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_LocDegraderMapperTests, nonMatchStringsAreNotDegraded)
{
	std::stringstream input;
	input << R"(link = { utf8 = "a" win1252= "b" })";
	const mappers::LocDegraderMapper mapper(input);

	EXPECT_EQ("cdefgh", mapper.degradeString("cdefgh"));
}

TEST(Mappers_LocDegraderMapperTests, matchedStringsAreDegraded)
{
	std::stringstream input;
	input << R"(link = { utf8 = "a" win1252= "b" })";
	const mappers::LocDegraderMapper mapper(input);

	EXPECT_EQ("bbcdefgh", mapper.degradeString("abcdefgh"));
}

TEST(Mappers_LocDegraderMapperTests, multipleMatchesAreReplaced)
{
	std::stringstream input;
	input << R"(link = { utf8 = "a" win1252= "b" })";
	input << R"(link = { utf8 = "c" win1252= "d" })";
	const mappers::LocDegraderMapper mapper(input);

	EXPECT_EQ("bbddefghbbddefgh", mapper.degradeString("abcdefghabcdefgh"));
}
