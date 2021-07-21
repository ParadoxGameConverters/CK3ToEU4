#include "../../CK3toEU4/Source/Mappers/IslamOverrideMapper/IslamOverrideMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_IslamOverrideMapperTests, nonMatchGivesEmptyOptional)
{
	std::stringstream input;
	input << "link = { province = 1 religion = test }";
	const mappers::IslamOverrideMapper mapper(input);

	EXPECT_EQ(std::nullopt, mapper.getReplacementReligionForProvince(2));
}

TEST(Mappers_IslamOverrideMapperTests, matchGivesReplacementReligion)
{
	std::stringstream input;
	input << "link = { province = 1 religion = test }";
	const mappers::IslamOverrideMapper mapper(input);

	EXPECT_EQ("test", mapper.getReplacementReligionForProvince(1));
}
