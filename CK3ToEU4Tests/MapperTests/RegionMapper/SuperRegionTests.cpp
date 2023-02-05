#include "../../CK3ToEU4/Source/Mappers/RegionMapper/Area.h"
#include "../../CK3ToEU4/Source/Mappers/RegionMapper/Region.h"
#include "../../CK3ToEU4/Source/Mappers/RegionMapper/SuperRegion.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_SuperRegionTests, blankSuperRegionLoadsWithNoRegions)
{
	const std::vector<std::string> input;
	const mappers::SuperRegion superRegion(input);

	ASSERT_TRUE(superRegion.getRegions().empty());
}

TEST(Mappers_SuperRegionTests, RegionsCanBeLoaded)
{
	const std::vector<std::string> input = {"region1", "region2"};
	const mappers::SuperRegion superRegion(input);

	ASSERT_FALSE(superRegion.getRegions().empty());
	ASSERT_EQ("region1", superRegion.getRegions().find("region1")->first);
}

TEST(Mappers_SuperRegionTests, superRegionCanBeLinkedToRegion)
{
	std::stringstream input;
	input << "areas = { test_area area2 area3 } \n";
	auto theRegion = std::make_shared<mappers::Region>(input);

	const std::vector<std::string> input2 = {"region1"};
	mappers::SuperRegion superRegion(input2);

	ASSERT_FALSE(superRegion.getRegions().find("region1")->second);
	superRegion.linkRegion(std::pair("region1", theRegion));
	ASSERT_TRUE(superRegion.getRegions().find("region1")->second);
}

TEST(Mappers_SuperRegionTests, LinkedRegionCanLocateProvince)
{
	std::stringstream input2;
	input2 << "{ 3 4 5 6 } \n";
	auto area2 = std::make_shared<mappers::Area>(input2);

	std::stringstream input;
	input << "areas = { area2 } \n";
	auto theRegion = std::make_shared<mappers::Region>(input);
	theRegion->linkArea(std::pair("area2", area2));

	const std::vector<std::string> input3 = {"region1"};
	mappers::SuperRegion superRegion(input3);
	superRegion.linkRegion(std::pair("region1", theRegion));

	ASSERT_TRUE(superRegion.superRegionContainsProvince(5));
}

TEST(Mappers_SuperRegionTests, LinkedRegionReturnsFalseOnProvinceMismatch)
{
	std::stringstream input2;
	input2 << "{ 3 4 5 6 } \n";
	auto area2 = std::make_shared<mappers::Area>(input2);

	std::stringstream input;
	input << "areas = { area2 } \n";
	auto theRegion = std::make_shared<mappers::Region>(input);
	theRegion->linkArea(std::pair("area2", area2));

	const std::vector<std::string> input3 = {"region1"};
	mappers::SuperRegion superRegion(input3);
	superRegion.linkRegion(std::pair("region1", theRegion));

	ASSERT_FALSE(superRegion.superRegionContainsProvince(9));
}
