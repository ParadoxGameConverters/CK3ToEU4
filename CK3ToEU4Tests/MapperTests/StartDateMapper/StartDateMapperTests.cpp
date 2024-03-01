#include "../../CK3ToEU4/Source/Mappers/StartDateMapper/StartDateMapper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_StartDateMapperTests, startDateDefaultsToDefault)
{
	std::stringstream input;

	const mappers::StartDateMapper theMapper(input);

	ASSERT_TRUE(theMapper.getStartDate().isSet());
	EXPECT_EQ(theMapper.getStartDate().getYear(), 1444);
	EXPECT_EQ(theMapper.getStartDate().getMonth(), 11);
	EXPECT_EQ(theMapper.getStartDate().getDay(), 11);
}

TEST(Mappers_StartDateMapperTests, startDateCanBeSet)
{
	std::stringstream input;
	input << "year = 1425\n";
	input << "month = 7\n";
	input << "day = 19\n";

	const mappers::StartDateMapper theMapper(input);

	ASSERT_TRUE(theMapper.getStartDate().isSet());
	EXPECT_EQ(theMapper.getStartDate().getYear(), 1425);
	EXPECT_EQ(theMapper.getStartDate().getMonth(), 7);
	EXPECT_EQ(theMapper.getStartDate().getDay(), 19);
}

TEST(Mappers_StartDateMapperTests, invalidYearBailsToDefault)
{
	std::stringstream input;
	input << "year = 1895\n";
	input << "month = 7\n";
	input << "day = 19\n";

	const mappers::StartDateMapper theMapper(input);

	ASSERT_TRUE(theMapper.getStartDate().isSet());
	EXPECT_EQ(theMapper.getStartDate().getYear(), 1444);
	EXPECT_EQ(theMapper.getStartDate().getMonth(), 11);
	EXPECT_EQ(theMapper.getStartDate().getDay(), 11);
}

TEST(Mappers_StartDateMapperTests, invalidMonthBailsToDefault)
{
	std::stringstream input;
	input << "year = 1495\n";
	input << "month = 13\n";
	input << "day = 19\n";

	const mappers::StartDateMapper theMapper(input);

	ASSERT_TRUE(theMapper.getStartDate().isSet());
	EXPECT_EQ(theMapper.getStartDate().getYear(), 1444);
	EXPECT_EQ(theMapper.getStartDate().getMonth(), 11);
	EXPECT_EQ(theMapper.getStartDate().getDay(), 11);
}

TEST(Mappers_StartDateMapperTests, invalidDayBailsToDefault)
{
	std::stringstream input;
	input << "year = 1495\n";
	input << "month = 12\n";
	input << "day = 0\n";

	const mappers::StartDateMapper theMapper(input);

	ASSERT_TRUE(theMapper.getStartDate().isSet());
	EXPECT_EQ(theMapper.getStartDate().getYear(), 1444);
	EXPECT_EQ(theMapper.getStartDate().getMonth(), 11);
	EXPECT_EQ(theMapper.getStartDate().getDay(), 11);
}

TEST(Mappers_StartDateMapperTests, invalidFebruaryBailsToDefault)
{
	std::stringstream input;
	input << "year = 1495\n";
	input << "month = 2\n";
	input << "day = 30\n";

	const mappers::StartDateMapper theMapper(input);

	ASSERT_TRUE(theMapper.getStartDate().isSet());
	EXPECT_EQ(theMapper.getStartDate().getYear(), 1444);
	EXPECT_EQ(theMapper.getStartDate().getMonth(), 11);
	EXPECT_EQ(theMapper.getStartDate().getDay(), 11);
}

TEST(Mappers_StartDateMapperTests, invalidThirtyDayMonthBailsToDefault)
{
	std::stringstream input;
	input << "year = 1495\n";
	input << "month = 4\n";
	input << "day = 31\n";

	const mappers::StartDateMapper theMapper(input);

	ASSERT_TRUE(theMapper.getStartDate().isSet());
	EXPECT_EQ(theMapper.getStartDate().getYear(), 1444);
	EXPECT_EQ(theMapper.getStartDate().getMonth(), 11);
	EXPECT_EQ(theMapper.getStartDate().getDay(), 11);
}
