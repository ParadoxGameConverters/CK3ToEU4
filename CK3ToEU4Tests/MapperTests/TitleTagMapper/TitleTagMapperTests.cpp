#include "../CK3ToEU4/Source/Mappers/TitleTagMapper/TitleTagMapper.h"
#include "gtest/gtest.h"
#include <sstream>


TEST(Mappers_TitleTagMapperTests, canMatchTagsOnTitles)
{
	std::stringstream input;
	input << "link = { eu4 = TST ck3 = c_test capitals = { 1 2 } }\n";
	input << "link = { eu4 = TST2 ck3 = c_test2 capitals = { 3 4 } }";

	mappers::TitleTagMapper theMapper(input);
	const auto& match = theMapper.getTagForTitle("c_test2");

	ASSERT_EQ("TST2", *match);
}

TEST(Mappers_TitleTagMapperTests, canMatchTagsOnTitlesAndCapitals)
{
	std::stringstream input;
	input << "link = { eu4 = TST ck3 = c_test capitals = { 1 2 } }\n";
	input << "link = { eu4 = TST2 ck3 = c_test2 capitals = { 3 4 } }";

	mappers::TitleTagMapper theMapper(input);
	const auto& match = theMapper.getTagForTitle("c_test2", 3);

	ASSERT_EQ("TST2", *match);
}

TEST(Mappers_TitleTagMapperTests, capitalsOverrideTitles)
{
	std::stringstream input;
	input << "link = { eu4 = TST ck3 = c_test capitals = { 1 2 } }\n";
	input << "link = { eu4 = TST2 ck3 = c_test2 capitals = { 3 4 } }";

	mappers::TitleTagMapper theMapper(input);
	const auto& match = theMapper.getTagForTitle("c_test", 3); // capital 3 is more relevant than title in a match.

	ASSERT_EQ("TST2", *match);
}

TEST(Mappers_TitleTagMapperTests, canGenerateNewTagsForMismatches)
{
	std::stringstream input;
	input << "link = { eu4 = TST ck3 = c_test capitals = { 1 2 } }\n";
	input << "link = { eu4 = TST2 ck3 = c_test2 capitals = { 3 4 } }";

	mappers::TitleTagMapper theMapper(input);
	const auto& match = theMapper.getTagForTitle("x_x_1", 0);
	const auto& match2 = theMapper.getTagForTitle("x_x_2", 0);
	const auto& match3 = theMapper.getTagForTitle("x_x_3", 0);

	ASSERT_EQ("Z00", *match);
	ASSERT_EQ("Z01", *match2);
	ASSERT_EQ("Z02", *match3);
}

TEST(Mappers_TitleTagMapperTests, canRegisterAllAccessedTitles)
{
	std::stringstream input;
	input << "link = { eu4 = TST ck3 = c_test capitals = { 1 2 } }\n";
	input << "link = { eu4 = TST2 ck3 = c_test2 capitals = { 3 4 } }";

	mappers::TitleTagMapper theMapper(input);
	const auto& match = theMapper.getTagForTitle("x_x_4");
	const auto& match2 = theMapper.getTagForTitle("x_x_5");
	const auto& match3 = theMapper.getTagForTitle("x_x_6", 2);
	const auto& match4 = theMapper.getTagForTitle("x_x_7", 0);
	const auto& match5 = theMapper.getTagForTitle("c_test2", 11);
	const auto& match6 = theMapper.getTagForTitle("x_x_8", 34);

	ASSERT_EQ("Z00", *match);
	ASSERT_EQ("Z01", *match2);
	ASSERT_EQ("TST", *match3);
	ASSERT_EQ("Z02", *match4);
	ASSERT_EQ("TST2", *match5);
	ASSERT_EQ("Z03", *match6);
}


TEST(Mappers_TitleTagMapperTests, ThePOPEGetsSpecialTreatment)
{
	std::stringstream input;
	input << "link = { eu4 = TST ck3 = c_test capitals = { 1 2 } }\n";
	input << "link = { eu4 = TST2 ck3 = c_test2 capitals = { 3 4 } }";

	mappers::TitleTagMapper theMapper(input);
	const auto& match = theMapper.getTagForTitle("x_x_4");
	const auto& match2 = theMapper.getTagForTitle("x_x_5");
	const auto& match3 = theMapper.getTagForTitle("The Pope", 2);
	const auto& match4 = theMapper.getTagForTitle("x_x_7", 0);
	const auto& match5 = theMapper.getTagForTitle("c_test2", 11);
	const auto& match6 = theMapper.getTagForTitle("x_x_8", 34);

	ASSERT_EQ("Z00", *match);
	ASSERT_EQ("Z01", *match2);
	ASSERT_EQ("PAP", *match3);
	ASSERT_EQ("Z02", *match4);
	ASSERT_EQ("TST2", *match5);
	ASSERT_EQ("Z03", *match6);
}

TEST(Mappers_TitleTagMapperTests, ThePOPEDoesNotUseUpValidMapping)
{
	std::stringstream input;
	input << "link = { eu4 = TST ck3 = c_test capitals = { 1 2 } }\n";
	input << "link = { eu4 = TST2 ck3 = c_test2 capitals = { 3 4 } }";

	mappers::TitleTagMapper theMapper(input);
	const auto& match = theMapper.getTagForTitle("x_x_4");
	const auto& match2 = theMapper.getTagForTitle("x_x_5");
	const auto& match3 = theMapper.getTagForTitle("The Pope", 2); // pope's capital is not relevant
	const auto& match4 = theMapper.getTagForTitle("x_x_7", 0);
	const auto& match5 = theMapper.getTagForTitle("c_test2", 2); // will get TST due to capital
	const auto& match6 = theMapper.getTagForTitle("x_x_8", 34);

	ASSERT_EQ("Z00", *match);
	ASSERT_EQ("Z01", *match2);
	ASSERT_EQ("PAP", *match3);
	ASSERT_EQ("Z02", *match4);
	ASSERT_EQ("TST", *match5);
	ASSERT_EQ("Z03", *match6);
}

TEST(Mappers_TitleTagMapperTests, ThePOPESubsequentTitlesGetDynamicTags)
{
	std::stringstream input;
	input << "link = { eu4 = TST ck3 = c_test capitals = { 1 2 } }\n";
	input << "link = { eu4 = TST2 ck3 = c_test2 capitals = { 3 4 } }";

	mappers::TitleTagMapper theMapper(input);
	const auto& match = theMapper.getTagForTitle("x_x_4");
	const auto& match2 = theMapper.getTagForTitle("x_x_5");
	const auto& match3 = theMapper.getTagForTitle("The Pope", 2); // pope's capital is not relevant
	const auto& match4 = theMapper.getTagForTitle("The Pope", 0);
	const auto& match5 = theMapper.getTagForTitle("The Pope", 2); // as demonstrated here
	const auto& match6 = theMapper.getTagForTitle("x_x_8", 34);

	ASSERT_EQ("Z00", *match);
	ASSERT_EQ("Z01", *match2);
	ASSERT_EQ("PAP", *match3);
	ASSERT_EQ("Z02", *match4);
	ASSERT_EQ("Z03", *match5);
	ASSERT_EQ("Z04", *match6);
}
