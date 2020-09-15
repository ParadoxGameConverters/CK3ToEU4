#include "../CK3toEU4/Source/Mappers/TitleTagMapper/TitleTagMapping.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_TitleTagMappingTests, FallbackMatchFailsWithoutEU4Tag)
{
	std::stringstream input;
	input << "= { fallback = yes }";

	const mappers::TitleTagMapping theMapper(input);

	ASSERT_FALSE(theMapper.fallbackMatch());
}

TEST(Mappers_TitleTagMappingTests, FallbackMatchSuccedsWithEU4Tag)
{
	std::stringstream input;
	input << "= { fallback = yes eu4 = TST }";

	const mappers::TitleTagMapping theMapper(input);

	ASSERT_TRUE(theMapper.fallbackMatch());
}

TEST(Mappers_TitleTagMappingTests, titleMatchFailsOnNoMatch)
{
	std::stringstream input;
	input << "= { ck3 = c_test eu4 = TST }";

	const mappers::TitleTagMapping theMapper(input);
	const auto& match = theMapper.titleMatch("c_fail");

	ASSERT_FALSE(match);
}

TEST(Mappers_TitleTagMappingTests, titleMatchSuceedsOnMatch)
{
	std::stringstream input;
	input << "= { ck3 = c_test eu4 = TST }";

	const mappers::TitleTagMapping theMapper(input);
	const auto& match = theMapper.titleMatch("c_test");

	ASSERT_EQ("TST", *match);
}

TEST(Mappers_TitleTagMappingTests, capitalsMatchFailsOnNoMatch)
{
	std::stringstream input;
	input << "= { capitals = { 1 2 3 } eu4 = TST }";

	const mappers::TitleTagMapping theMapper(input);
	const auto& match = theMapper.capitalMatch(4);

	ASSERT_FALSE(match);
}

TEST(Mappers_TitleTagMappingTests, capitalsMatchSuceedsOnMatch)
{
	std::stringstream input;
	input << "= { capitals = { 1 2 3 } eu4 = TST }";

	const mappers::TitleTagMapping theMapper(input);
	const auto& match = theMapper.capitalMatch(2);

	ASSERT_EQ("TST", *match);
}
