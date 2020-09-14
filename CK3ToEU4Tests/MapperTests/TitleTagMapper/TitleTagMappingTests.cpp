#include "../CK3toEU4/Source/Mappers/TitleTagMapper/TitleTagMapping.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_TitleTagMappingTests, EU4TagDefaultsToBlank)
{
	std::stringstream input;

	const mappers::TitleTagMapping theMapper(input);

	ASSERT_TRUE(theMapper.getEU4Tag().empty());
}

TEST(Mappers_TitleTagMappingTests, EU4TagCanBeAdded)
{
	std::stringstream input;
	input << "= { eu4 = TST }";

	const mappers::TitleTagMapping theMapper(input);

	ASSERT_EQ(theMapper.getEU4Tag(), "TST");
}

TEST(Mappers_TitleTagMappingTests, FallbackDefaultsToFalse)
{
	std::stringstream input;

	const mappers::TitleTagMapping theMapper(input);

	ASSERT_FALSE(theMapper.getFallback());
}

TEST(Mappers_TitleTagMappingTests, FallbackCanBeSet)
{
	std::stringstream input;
	input << "= { fallback = yes }";

	const mappers::TitleTagMapping theMapper(input);

	ASSERT_TRUE(theMapper.getFallback());
}

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

TEST(Mappers_TitleTagMappingTests, CK3TitleDefaultsToBlank)
{
	std::stringstream input;

	const mappers::TitleTagMapping theMapper(input);

	ASSERT_TRUE(theMapper.getCK3Title().empty());
}

TEST(Mappers_TitleTagMappingTests, CK3TitleCanBeAdded)
{
	std::stringstream input;
	input << "= { ck3 = c_test }";

	const mappers::TitleTagMapping theMapper(input);

	ASSERT_EQ(theMapper.getCK3Title(), "c_test");
}

TEST(Mappers_TitleTagMappingTests, capitalsDefaultToEmpty)
{
	std::stringstream input;

	const mappers::TitleTagMapping theMapper(input);

	ASSERT_TRUE(theMapper.getCapitals().empty());
}

TEST(Mappers_TitleTagMappingTests, CapitalsCanBeAdded)
{
	std::stringstream input;
	input << "= { capitals = { 1 2 3 3 2} }";

	const mappers::TitleTagMapping theMapper(input);

	ASSERT_EQ(theMapper.getCapitals().size(), 3);
	ASSERT_EQ(theMapper.getCapitals().count(1), 1);
	ASSERT_EQ(theMapper.getCapitals().count(2), 1);
	ASSERT_EQ(theMapper.getCapitals().count(3), 1);
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

	ASSERT_EQ(*match, "TST");
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

	ASSERT_EQ(*match, "TST");
}
