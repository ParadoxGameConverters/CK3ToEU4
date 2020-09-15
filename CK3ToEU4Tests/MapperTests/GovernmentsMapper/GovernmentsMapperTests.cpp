#include "../../CK3toEU4/Source/Mappers/GovernmentsMapper/GovernmentsMapper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_GovernmentsMapperTests, nonMatchGivesEmptyOptional)
{
	std::stringstream input;
	input << "link = { gov = eu4Government ck3gov = ck3Government }";

	const mappers::GovernmentsMapper theMapper(input);

	ASSERT_FALSE(theMapper.matchGovernment("nonMatchingGovernment", "ck3title"));
}


TEST(Mappers_GovernmentsMapperTests, eu4GovernmentCanBeFound)
{
	std::stringstream input;
	input << "link = { gov = eu4Government ck3gov = ck3Government }";

	const mappers::GovernmentsMapper theMapper(input);
	auto match = theMapper.matchGovernment("ck3Government", "ck3title");

	ASSERT_EQ("eu4Government", match->first);
}


TEST(Mappers_GovernmentsMapperTests, multipleCK3GovernmentsCanBeInARule)
{
	std::stringstream input;
	input << "link = { gov = eu4Government ck3gov = ck3Government ck3gov = ck3Government2 }";

	const mappers::GovernmentsMapper theMapper(input);
	auto match = theMapper.matchGovernment("ck3Government2", "ck3title");

	ASSERT_EQ("eu4Government", match->first);
}

TEST(Mappers_GovernmentsMapperTests, correctRuleMatches)
{
	std::stringstream input;
	input << "link = { gov = eu4Government ck3gov = ck3Government }\n";
	input << "link = { gov = eu4Government2 ck3gov = ck3Government2 }";

	const mappers::GovernmentsMapper theMapper(input);
	auto match = theMapper.matchGovernment("ck3Government2", "ck3title");

	ASSERT_EQ("eu4Government2", match->first);
}

TEST(Mappers_GovernmentsMapperTests, ck3TitleCanBeFound)
{
	std::stringstream input;
	input << "link = { gov = eu4Government ck3gov = ck3Government }\n";
	input << "link = { gov = eu4Government2 ck3gov = ck3Government2 ck3title = c_test }";

	const mappers::GovernmentsMapper theMapper(input);
	auto match = theMapper.matchGovernment("", "c_test");

	ASSERT_EQ("eu4Government2", match->first);
}

TEST(Mappers_GovernmentsMapperTests, ck3TitleTakesPriority)
{
	std::stringstream input;
	input << "link = { gov = eu4Government ck3gov = ck3Government }\n";
	input << "link = { gov = eu4Government2 ck3gov = ck3Government2 ck3title = c_test }";

	const mappers::GovernmentsMapper theMapper(input);
	auto match = theMapper.matchGovernment("ck3Government", "c_test");

	ASSERT_EQ("eu4Government2", match->first);
}

TEST(Mappers_GovernmentsMapperTests, reformIsReturnedIfExists)
{
	std::stringstream input;
	input << "link = { gov = eu4Government ck3gov = ck3Government }\n";
	input << "link = { gov = eu4Government2 ck3gov = ck3Government2 ck3title = c_test reform = papacy_reform }";

	const mappers::GovernmentsMapper theMapper(input);
	auto match = theMapper.matchGovernment("ck3Government", "c_test");

	ASSERT_EQ("eu4Government2", match->first);
	ASSERT_EQ("papacy_reform", match->second);
}

TEST(Mappers_GovernmentsMapperTests, reformIsEmptyIfDoesNotExist)
{
	std::stringstream input;
	input << "link = { gov = eu4Government ck3gov = ck3Government }\n";
	input << "link = { gov = eu4Government2 ck3gov = ck3Government2 ck3title = c_test }";

	const mappers::GovernmentsMapper theMapper(input);
	auto match = theMapper.matchGovernment("ck3Government", "c_test");

	ASSERT_TRUE(match->second.empty());
}