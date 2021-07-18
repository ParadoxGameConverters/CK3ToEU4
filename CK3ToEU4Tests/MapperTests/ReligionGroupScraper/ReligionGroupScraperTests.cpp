#include "../../CK3toEU4/Source/Mappers/ReligionGroupScraper/ReligionGroupScraper.h"
#include "../../CK3toEU4/Source/Mappers/ReligionGroupScraper/ReligionGroupScraping.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_ReligionGroupScraperTests, nonMatchGivesEmptyOptional)
{
	std::stringstream input;
	input << "religion_group_1 = { religion1={} religion2={} }\n";
	input << "religion_group_2 = { religion3={} religion4={} }\n";
	input << "religion_group_1 = { religion1={} religion5={} }\n";
	const mappers::ReligionGroupScraper theMapper(input);

	const auto& religionGroup = theMapper.getReligionGroupForReligion("nonsense");
	ASSERT_FALSE(religionGroup);
}

TEST(Mappers_ReligionGroupScraperTests, matchReturnsReligionGroup)
{
	std::stringstream input;
	input << "religion_group_1 = { religion1={} religion2={} }\n";
	input << "religion_group_2 = { religion3={} religion4={} }\n";
	input << "religion_group_1 = { religion1={} religion5={} }\n";
	const mappers::ReligionGroupScraper theMapper(input);

	const auto& religionGroup = theMapper.getReligionGroupForReligion("religion4");
	ASSERT_EQ("religion_group_2", *religionGroup);
}

TEST(Mappers_ReligionGroupScraperTests, matchReturnsFirstMatch)
{
	std::stringstream input;
	input << "religion_group_1 = { religion1={} religion2={} }\n";
	input << "religion_group_2 = { religion3={} religion4={} }\n";
	input << "religion_group_3 = { religion1={} religion5={} }\n";
	const mappers::ReligionGroupScraper theMapper(input);

	const auto& religionGroup = theMapper.getReligionGroupForReligion("religion1");
	ASSERT_EQ("religion_group_1", *religionGroup);
}
