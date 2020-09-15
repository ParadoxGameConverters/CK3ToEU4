#include "../../CK3toEU4/Source/Mappers/RegionMapper/RegionMapper.h"
#include "gtest/gtest.h"
#include <sstream>

// This is a collective mapper test for Area, Region, SuperRegion and RegionMapper
// submodules. They can't work without each other in concert.

TEST(Mappers_RegionMapperTests, regionMapperCanBeEnabled)
{
	// We start humble, it's a machine.
	mappers::RegionMapper theMapper;
	std::stringstream areaStream;
	std::stringstream regionStream;
	std::stringstream superRegionStream;

	theMapper.loadRegions(areaStream, regionStream, superRegionStream);
	ASSERT_FALSE(theMapper.provinceIsInRegion(1, "test"));
	ASSERT_FALSE(theMapper.regionNameIsValid("test"));
	ASSERT_FALSE(theMapper.getParentAreaName(1));
	ASSERT_FALSE(theMapper.getParentRegionName(1));
	ASSERT_FALSE(theMapper.getParentSuperRegionName(1));
}

TEST(Mappers_RegionMapperTests, loadingBrokenAreaWillThrowException)
{
	mappers::RegionMapper theMapper;
	std::stringstream areaStream;
	areaStream << "broken_area = { 1 2 3 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }";
	try
	{
		theMapper.loadRegions(areaStream, regionStream, superRegionStream);
		FAIL();
	}
	catch (const std::runtime_error& e)
	{
		ASSERT_STREQ("Region's test_region area test_area does not exist!", e.what());
	}
}

TEST(Mappers_RegionMapperTests, loadingBrokenRegionWillThrowException)
{
	mappers::RegionMapper theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } ";
	std::stringstream regionStream;
	regionStream << "broken_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }";
	try
	{
		theMapper.loadRegions(areaStream, regionStream, superRegionStream);
		FAIL();
	}
	catch (const std::runtime_error& e)
	{
		ASSERT_STREQ("Superregion's test_superregion region test_region does not exist!", e.what());
	}
}

TEST(Mappers_RegionMapperTests, locationServicesWork)
{
	mappers::RegionMapper theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	ASSERT_TRUE(theMapper.provinceIsInRegion(3, "test_area"));
	ASSERT_TRUE(theMapper.provinceIsInRegion(2, "test_region"));
	ASSERT_TRUE(theMapper.provinceIsInRegion(1, "test_superregion"));
}

TEST(Mappers_RegionMapperTests, locationServicesCorrectlyFail)
{
	mappers::RegionMapper theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	ASSERT_FALSE(theMapper.provinceIsInRegion(4, "test_area"));
	ASSERT_FALSE(theMapper.provinceIsInRegion(5, "test_region"));
	ASSERT_FALSE(theMapper.provinceIsInRegion(6, "test_superregion"));
}

TEST(Mappers_RegionMapperTests, locationServicesFailForNonsense)
{
	mappers::RegionMapper theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	ASSERT_FALSE(theMapper.provinceIsInRegion(1, "nonsense"));
	ASSERT_FALSE(theMapper.provinceIsInRegion(9, "test_area"));
}

TEST(Mappers_RegionMapperTests, correctParentLocationsReported)
{
	mappers::RegionMapper theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	ASSERT_EQ("test_area", *theMapper.getParentAreaName(1));
	ASSERT_EQ("test_region", *theMapper.getParentRegionName(1));
	ASSERT_EQ("test_superregion", *theMapper.getParentSuperRegionName(1));
	ASSERT_EQ("test_area2", *theMapper.getParentAreaName(5));
	ASSERT_EQ("test_region2", *theMapper.getParentRegionName(5));
	ASSERT_EQ("test_superregion2", *theMapper.getParentSuperRegionName(5));
}

TEST(Mappers_RegionMapperTests, wrongParentLocationsReturnNullopt)
{
	mappers::RegionMapper theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	ASSERT_FALSE(theMapper.getParentAreaName(7));
	ASSERT_FALSE(theMapper.getParentRegionName(7));
	ASSERT_FALSE(theMapper.getParentSuperRegionName(7));
}

TEST(Mappers_RegionMapperTests, locationNameValidationWorks)
{
	mappers::RegionMapper theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	ASSERT_TRUE(theMapper.regionNameIsValid("test_area"));
	ASSERT_TRUE(theMapper.regionNameIsValid("test_region2"));
	ASSERT_TRUE(theMapper.regionNameIsValid("test_superregion2"));
	ASSERT_FALSE(theMapper.regionNameIsValid("nonsense"));
}
