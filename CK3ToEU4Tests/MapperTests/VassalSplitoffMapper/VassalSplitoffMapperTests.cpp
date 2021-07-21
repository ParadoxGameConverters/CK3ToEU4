#include "../CK3ToEU4/Source/Mappers/VassalSplitoffMapper/VassalSplitoffMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_VassalSplitoffMapperTests, factorDefaultsTo1)
{
	std::stringstream input;
	const mappers::VassalSplitoffMapper mapper(input);

	EXPECT_DOUBLE_EQ(1.0, mapper.getFactor());
}

TEST(Mappers_VassalSplitoffMapperTests, factorCanBeHalved)
{
	std::stringstream input;
	input << "sensitivity = 200";
	const mappers::VassalSplitoffMapper mapper(input);

	EXPECT_DOUBLE_EQ(0.5, mapper.getFactor());
}

TEST(Mappers_VassalSplitoffMapperTests, factorCanBeDoubled)
{
	std::stringstream input;
	input << "sensitivity = 50";
	const mappers::VassalSplitoffMapper mapper(input);

	EXPECT_DOUBLE_EQ(2, mapper.getFactor());
}

TEST(Mappers_VassalSplitoffMapperTests, outOfRangeDefaultsTo1)
{
	std::stringstream input;
	input << "sensitivity = 0";
	const mappers::VassalSplitoffMapper mapper(input);

	EXPECT_DOUBLE_EQ(1, mapper.getFactor());

	std::stringstream input2;
	input2 << "sensitivity = 1000000";
	const mappers::VassalSplitoffMapper mapper2(input2);

	EXPECT_DOUBLE_EQ(1, mapper2.getFactor());
}
