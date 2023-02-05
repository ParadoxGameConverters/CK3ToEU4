#include "../../CK3ToEU4/Source/Mappers/CultureMapper/CultureMapper.h"
#include "../../CK3ToEU4/Source/Mappers/RegionMapper/RegionMapper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_CultureMapperTests, nonMatchGivesEmptyOptional)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = culture }";
	const mappers::CultureMapper culMapper(input);

	ASSERT_FALSE(culMapper.cultureMatch("nonMatchingCulture", "", 0, ""));
}

TEST(Mappers_CultureMapperTests, simpleCultureMatches)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = test }";
	const mappers::CultureMapper culMapper(input);

	ASSERT_EQ("culture", *culMapper.cultureMatch("test", "", 0, ""));
}

TEST(Mappers_CultureMapperTests, simpleCultureCorrectlyMatches)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi }";
	const mappers::CultureMapper culMapper(input);

	ASSERT_EQ("culture", *culMapper.cultureMatch("test", "", 0, ""));
}

TEST(Mappers_CultureMapperTests, cultureMatchesWithReligion)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi religion = thereligion }";
	const mappers::CultureMapper culMapper(input);

	ASSERT_EQ("culture", *culMapper.cultureMatch("test", "thereligion", 0, ""));
}

TEST(Mappers_CultureMapperTests, cultureFailsWithWrongReligion)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi religion = thereligion }";
	const mappers::CultureMapper culMapper(input);

	ASSERT_FALSE(culMapper.cultureMatch("test", "unreligion", 0, ""));
}

TEST(Mappers_CultureMapperTests, cultureMatchesWithCapital)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi province = 4 }";
	const mappers::CultureMapper culMapper(input);

	ASSERT_EQ("culture", *culMapper.cultureMatch("test", "", 4, ""));
}

TEST(Mappers_CultureMapperTests, cultureFailsWithWrongCapital)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi province = 4 }";
	const mappers::CultureMapper culMapper(input);

	ASSERT_FALSE(culMapper.cultureMatch("test", "", 3, ""));
}

TEST(Mappers_CultureMapperTests, cultureMatchesWithOwnerTag)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi owner = TAG }";
	const mappers::CultureMapper culMapper(input);

	ASSERT_EQ("culture", *culMapper.cultureMatch("test", "", 0, "TAG"));
}

TEST(Mappers_CultureMapperTests, cultureFailsWithWrongTag)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi owner = TAG }";
	const mappers::CultureMapper culMapper(input);

	ASSERT_FALSE(culMapper.cultureMatch("test", "", 0, "GAT"));
}

TEST(Mappers_CultureMapperTests, cultureMapperThrowsExceptionWithoutRegionMapper)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi region = atlantis }";
	const mappers::CultureMapper culMapper(input);
	try
	{
		auto attempt = culMapper.cultureMatch("test", "", 4, "");
		FAIL();
	}
	catch (const std::runtime_error& e)
	{
		ASSERT_STREQ("Culture Mapper: Region Mapper is unloaded!", e.what());
	}
}

TEST(Mappers_CultureMapperTests, cultureMapperFailsForInvalidRegion)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi religion = thereligion owner = TAG region = atlantis }";
	mappers::CultureMapper culMapper(input);

	auto theMapper = std::make_shared<mappers::RegionMapper>();
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper->loadRegions(areaStream, regionStream, superRegionStream);

	culMapper.loadRegionMapper(theMapper);
	ASSERT_FALSE(culMapper.cultureMatch("test", "", 7, ""));
}

TEST(Mappers_CultureMapperTests, cultureMapperMatchesOnRegion)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi region = test_superregion }";
	mappers::CultureMapper culMapper(input);

	auto theMapper = std::make_shared<mappers::RegionMapper>();
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper->loadRegions(areaStream, regionStream, superRegionStream);

	culMapper.loadRegionMapper(theMapper);

	ASSERT_EQ("culture", *culMapper.cultureMatch("test", "", 1, ""));
}

TEST(Mappers_CultureMapperTests, cultureMapperFailsOnWrongRegion)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi region = test_superregion }";
	mappers::CultureMapper culMapper(input);

	auto theMapper = std::make_shared<mappers::RegionMapper>();
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper->loadRegions(areaStream, regionStream, superRegionStream);

	culMapper.loadRegionMapper(theMapper);

	ASSERT_FALSE(culMapper.cultureMatch("test", "", 6, ""));
}

TEST(Mappers_CultureMapperTests, TechGroupCanBeRetrieved)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi religion = thereligion tech = the_tech }";
	const mappers::CultureMapper culMapper(input);

	ASSERT_EQ("the_tech", *culMapper.getTechGroup("culture"));
}

TEST(Mappers_CultureMapperTests, techGroupFailsForNonsenseCulture)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi religion = thereligion tech = the_tech }";
	const mappers::CultureMapper culMapper(input);

	ASSERT_FALSE(culMapper.getTechGroup("nonsense"));
}

TEST(Mappers_CultureMapperTests, techGroupFailsForMissingTechEntry)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi religion = thereligion }";
	const mappers::CultureMapper culMapper(input);

	ASSERT_FALSE(culMapper.getTechGroup("culture"));
}


TEST(Mappers_CultureMapperTests, GFXCanBeRetrieved)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi religion = thereligion gfx = gfxtest }";
	const mappers::CultureMapper culMapper(input);

	ASSERT_EQ("gfxtest", *culMapper.getGFX("culture"));
}

TEST(Mappers_CultureMapperTests, GFXFailsForNonsenseCulture)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi religion = thereligion gfx = gfxtest }";
	const mappers::CultureMapper culMapper(input);

	ASSERT_FALSE(culMapper.getGFX("nonsense"));
}

TEST(Mappers_CultureMapperTests, GFXFailsForMissingTechEntry)
{
	std::stringstream input;
	input << "link = { eu4 = culture ck3 = qwe ck3 = test ck3 = poi religion = thereligion}";
	const mappers::CultureMapper culMapper(input);

	ASSERT_FALSE(culMapper.getGFX("culture"));
}
