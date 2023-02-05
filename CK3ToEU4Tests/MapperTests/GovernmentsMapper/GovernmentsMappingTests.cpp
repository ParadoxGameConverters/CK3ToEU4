#include "../../CK3ToEU4/Source/Mappers/GovernmentsMapper/GovernmentsMapping.h"
#include "gtest/gtest.h"
#include <sstream>


TEST(Mappers_GovernmentsMappingTests, eu4GovernmentDefaultsToBlank)
{
	std::stringstream input;
	input << "= {}";

	const mappers::GovernmentsMapping theMapping(input);

	ASSERT_TRUE(theMapping.getGovernment().empty());
}

TEST(Mappers_GovernmentsMappingTests, eu4GovernmentCanBeSet)
{
	std::stringstream input;
	input << "= { gov = eu4Government }";

	const mappers::GovernmentsMapping theMapping(input);

	ASSERT_EQ("eu4Government", theMapping.getGovernment());
}

TEST(Mappers_GovernmentsMappingTests, ck3GovernmentsDefaultToEmpty)
{
	std::stringstream input;
	input << "= {}";

	const mappers::GovernmentsMapping theMapping(input);

	ASSERT_TRUE(theMapping.getCK3Governments().empty());
}

TEST(Mappers_GovernmentsMappingTests, ck3GovernmentsCanBeSet)
{
	std::stringstream input;
	input << "= { ck3gov = religion1 ck3gov = religion2 }";

	const mappers::GovernmentsMapping theMapping(input);

	ASSERT_EQ(2, theMapping.getCK3Governments().size());
	ASSERT_EQ("religion1", *theMapping.getCK3Governments().find("religion1"));
	ASSERT_EQ("religion2", *theMapping.getCK3Governments().find("religion2"));
}

TEST(Mappers_GovernmentsMappingTests, ck3titleDefaultToEmpty)
{
	std::stringstream input;
	input << "= {}";

	const mappers::GovernmentsMapping theMapping(input);

	ASSERT_TRUE(theMapping.getCK3Title().empty());
}

TEST(Mappers_GovernmentsMappingTests, ck3TitleCanBeSet)
{
	std::stringstream input;
	input << "= { ck3title = c_test }";

	const mappers::GovernmentsMapping theMapping(input);

	ASSERT_EQ("c_test", theMapping.getCK3Title());
}

TEST(Mappers_GovernmentsMappingTests, reformDefaultToEmpty)
{
	std::stringstream input;
	input << "= {}";

	const mappers::GovernmentsMapping theMapping(input);

	ASSERT_TRUE(theMapping.getReform().empty());
}

TEST(Mappers_GovernmentsMappingTests, reformCanBeSet)
{
	std::stringstream input;
	input << "= { reform = theReform }";

	const mappers::GovernmentsMapping theMapping(input);

	ASSERT_EQ("theReform", theMapping.getReform());
}