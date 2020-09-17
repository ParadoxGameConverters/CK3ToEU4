#include "../CK3toEU4/Source/Mappers/DevWeightsMapper/DevWeightsMapper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_DevWeightsMapperTests, devFromBuildingDefaultsToZero)
{
	std::stringstream input;

	const mappers::DevWeightsMapper theMapper(input);

	ASSERT_NEAR(0, theMapper.getDevFromBuilding(), 0.001);
}

TEST(Mappers_DevWeightsMapperTests, devFromBuildingCanBeSet)
{
	std::stringstream input;
	input << "dev_from_building = 0.3\n";

	const mappers::DevWeightsMapper theMapper(input);

	ASSERT_NEAR(0.3, theMapper.getDevFromBuilding(), 0.001);
}

TEST(Mappers_DevWeightsMapperTests, devFromHoldingDefaultsToZero)
{
	std::stringstream input;

	const mappers::DevWeightsMapper theMapper(input);

	ASSERT_NEAR(0, theMapper.getDevFromHolding(), 0.001);
}

TEST(Mappers_DevWeightsMapperTests, devFromHoldingCanBeSet)
{
	std::stringstream input;
	input << "dev_from_holding = 0.7\n";

	const mappers::DevWeightsMapper theMapper(input);

	ASSERT_NEAR(0.7, theMapper.getDevFromHolding(), 0.001);
}

TEST(Mappers_DevWeightsMapperTests, devFromDevelopmentDefaultsToZero)
{
	std::stringstream input;

	const mappers::DevWeightsMapper theMapper(input);

	ASSERT_NEAR(0, theMapper.getDevFromDev(), 0.001);
}

TEST(Mappers_DevWeightsMapperTests, devFromDevelopmentCanBeSet)
{
	std::stringstream input;
	input << "dev_from_development = 0.7\n";

	const mappers::DevWeightsMapper theMapper(input);

	ASSERT_NEAR(0.7, theMapper.getDevFromDev(), 0.001);
}
