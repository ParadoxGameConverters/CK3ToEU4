#include "../../CK3toEU4/Source/Mappers/ReligionMapper/ReligionMapping.h"
#include "gtest/gtest.h"
#include <sstream>


TEST(Mappers_ReligionMappingTests, eu4ReligionDefaultsToBlank)
{
	std::stringstream input;
	input << "= {}";

	const mappers::ReligionMapping theMapping(input);

	ASSERT_TRUE(theMapping.getEU4Religion().empty());
}


TEST(Mappers_ReligionMappingTests, eu4ReligionCanBeSet)
{
	std::stringstream input;
	input << "= { eu4 = eu4Religion }";

	const mappers::ReligionMapping theMapping(input);

	ASSERT_EQ(theMapping.getEU4Religion(), "eu4Religion");
}


TEST(Mappers_ReligionMappingTests, ck3ReligionsDefaultToEmpty)
{
	std::stringstream input;
	input << "= {}";

	const mappers::ReligionMapping theMapping(input);

	ASSERT_TRUE(theMapping.getCK3Religions().empty());
}


TEST(Mappers_ReligionMappingTests, ck3ReligionsCanBeSet)
{
	std::stringstream input;
	input << "= { ck3 = religion1 ck3 = religion2 }";

	const mappers::ReligionMapping theMapping(input);

	ASSERT_EQ(theMapping.getCK3Religions().size(), 2);
	ASSERT_EQ(*theMapping.getCK3Religions().find("religion1"), "religion1");
	ASSERT_EQ(*theMapping.getCK3Religions().find("religion2"), "religion2");
}