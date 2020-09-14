#include "../../CK3toEU4/Source/Mappers/ReligionMapper/ReligionMapper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_ReligionMapperTests, nonMatchGivesEmptyOptional)
{
	std::stringstream input;
	input << "link = { eu4 = eu4Religion ck3 = ck3Religion }";

	const mappers::ReligionMapper theMapper(input);

	const auto& eu4Religion = theMapper.getEU4ReligionForCK3Religion("nonMatchingReligion");
	ASSERT_FALSE(eu4Religion);
}


TEST(Mappers_ReligionMapperTests, eu4ReligionCanBeFound)
{
	std::stringstream input;
	input << "link = { eu4 = eu4Religion ck3 = ck3Religion }";

	const mappers::ReligionMapper theMapper(input);

	const auto& eu4Religion = theMapper.getEU4ReligionForCK3Religion("ck3Religion");
	ASSERT_EQ(eu4Religion, "eu4Religion");
}


TEST(Mappers_ReligionMapperTests, multipleEU4ReligionsCanBeInARule)
{
	std::stringstream input;
	input << "link = { eu4 = eu4Religion ck3 = ck3Religion ck3 = ck3Religion2 }";

	const mappers::ReligionMapper theMapper(input);

	const auto& eu4Religion = theMapper.getEU4ReligionForCK3Religion("ck3Religion2");
	ASSERT_EQ(eu4Religion, "eu4Religion");
}


TEST(Mappers_ReligionMapperTests, correctRuleMatches)
{
	std::stringstream input;
	input << "link = { eu4 = eu4Religion ck3 = ck3Religion }";
	input << "link = { eu4 = eu4Religion2 ck3 = ck3Religion2 }";

	const mappers::ReligionMapper theMapper(input);

	const auto& eu4Religion = theMapper.getEU4ReligionForCK3Religion("ck3Religion2");
	ASSERT_EQ(eu4Religion, "eu4Religion2");
}