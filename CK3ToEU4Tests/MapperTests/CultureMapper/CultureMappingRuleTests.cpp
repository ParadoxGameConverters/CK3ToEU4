#include "../../CK3ToEU4/Source/Mappers/CultureMapper/CultureMappingRule.h"
#include "../../CK3ToEU4/Source/Mappers/RegionMapper/RegionMapper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_CultureMappingRuleTests, primitivesDefaultsToBlank)
{
	std::stringstream input;
	input << "= {}";

	const mappers::CultureMappingRule theMapping(input);

	ASSERT_TRUE(theMapping.getEU4Culture().empty());
	ASSERT_TRUE(theMapping.getCK3Cultures().empty());
	ASSERT_TRUE(theMapping.getReligions().empty());
	ASSERT_TRUE(theMapping.getReligions().empty());
	ASSERT_TRUE(theMapping.getOwners().empty());
	ASSERT_TRUE(theMapping.getProvinces().empty());
	ASSERT_TRUE(theMapping.getTechGroupField().empty());
	ASSERT_TRUE(theMapping.getGFXField().empty());
}

TEST(Mappers_CultureMappingRuleTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "= { eu4 = eu4Culture ck3 = culture1 ck3 = culture2 religion = religion1 religion = religion2";
	input << " region = region1 region = region2 owner = TAG1 owner = TAG2 province = 3 province = 4";
	input << " tech = high_american gfx = test }";

	const mappers::CultureMappingRule theMapping(input);

	ASSERT_EQ("eu4Culture", theMapping.getEU4Culture());
	ASSERT_EQ(theMapping.getCK3Cultures().size(), 2);
	ASSERT_EQ(*theMapping.getCK3Cultures().find("culture1"), "culture1");
	ASSERT_EQ(*theMapping.getCK3Cultures().find("culture2"), "culture2");
	ASSERT_EQ(theMapping.getReligions().size(), 2);
	ASSERT_EQ("religion1", *theMapping.getReligions().find("religion1"));
	ASSERT_EQ("religion2", *theMapping.getReligions().find("religion2"));
	ASSERT_EQ(theMapping.getRegions().size(), 2);
	ASSERT_EQ("region1", *theMapping.getRegions().find("region1"));
	ASSERT_EQ("region2", *theMapping.getRegions().find("region2"));
	ASSERT_EQ(theMapping.getOwners().size(), 2);
	ASSERT_EQ("TAG1", *theMapping.getOwners().find("TAG1"));
	ASSERT_EQ("TAG2", *theMapping.getOwners().find("TAG2"));
	ASSERT_EQ(2, theMapping.getProvinces().size());
	ASSERT_EQ(3, *theMapping.getProvinces().find(3));
	ASSERT_EQ(4, *theMapping.getProvinces().find(4));
	ASSERT_EQ("high_american", theMapping.getTechGroupField());
	ASSERT_EQ("test", theMapping.getGFXField());
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnSimpleCulture)
{
	std::stringstream input;
	input << "eu4 = flemish ck3 = dutch";
	const mappers::CultureMappingRule theMapping(input);

	ASSERT_EQ("flemish", *theMapping.cultureMatch("dutch", "", 0, ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnSimpleCultureFailsForWrongCulture)
{
	std::stringstream input;
	input << "eu4 = flemish ck3 = flemish";
	const mappers::CultureMappingRule theMapping(input);

	ASSERT_FALSE(theMapping.cultureMatch("dutch", "", 0, ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnReligion)
{
	std::stringstream input;
	input << "eu4 = sephardi ck3 = german religion = jewish";
	const mappers::CultureMappingRule theMapping(input);

	ASSERT_EQ("sephardi", *theMapping.cultureMatch("german", "jewish", 0, ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnReligionFailsForNoReligion)
{
	std::stringstream input;
	input << "eu4 = sephardi ck3 = german religion = jewish";
	const mappers::CultureMappingRule theMapping(input);

	ASSERT_FALSE(theMapping.cultureMatch("german", "", 0, ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnReligionFailsForWrongReligion)
{
	std::stringstream input;
	input << "eu4 = sephardi ck3 = german religion = jewish";
	const mappers::CultureMappingRule theMapping(input);

	ASSERT_FALSE(theMapping.cultureMatch("german", "sunni", 0, ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnOwner)
{
	std::stringstream input;
	input << "eu4 = dutch ck3 = german owner = NED";
	const mappers::CultureMappingRule theMapping(input);

	ASSERT_EQ("dutch", *theMapping.cultureMatch("german", "", 0, "NED"));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnOwnerFailsForWrongOwner)
{
	std::stringstream input;
	input << "eu4 = dutch ck3 = german owner = NED";
	const mappers::CultureMappingRule theMapping(input);

	ASSERT_FALSE(theMapping.cultureMatch("german", "", 0, "FRA"));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnOwnerFailsForNoOwner)
{
	std::stringstream input;
	input << "eu4 = dutch ck3 = german owner = NED";
	const mappers::CultureMappingRule theMapping(input);

	ASSERT_FALSE(theMapping.cultureMatch("german", "", 0, ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnProvince)
{
	std::stringstream input;
	input << "eu4 = dutch ck3 = german province = 17";
	const mappers::CultureMappingRule theMapping(input);

	ASSERT_EQ("dutch", *theMapping.cultureMatch("german", "", 17, ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnProvinceFailsForWrongProvince)
{
	std::stringstream input;
	input << "eu4 = dutch ck3 = german province = 17";
	const mappers::CultureMappingRule theMapping(input);

	ASSERT_FALSE(theMapping.cultureMatch("german", "", 19, ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnProvinceFailsForNoProvince)
{
	std::stringstream input;
	input << "eu4 = dutch ck3 = german province = 17";
	const mappers::CultureMappingRule theMapping(input);

	ASSERT_FALSE(theMapping.cultureMatch("german", "", 0, ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnRegion)
{
	auto theMapper = std::make_shared<mappers::RegionMapper>();
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 17 } ";
	std::stringstream regionStream;
	regionStream << "dutch_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { dutch_region }";
	theMapper->loadRegions(areaStream, regionStream, superRegionStream);

	std::stringstream input;
	input << "eu4 = dutch ck3 = german region = dutch_region";
	mappers::CultureMappingRule theMapping(input);
	theMapping.insertRegionMapper(theMapper);

	ASSERT_EQ("dutch", *theMapping.cultureMatch("german", "", 17, ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnRegionFailsForWrongRegion)
{
	auto theMapper = std::make_shared<mappers::RegionMapper>();
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 17 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }";
	theMapper->loadRegions(areaStream, regionStream, superRegionStream);

	std::stringstream input;
	input << "eu4 = dutch ck3 = german region = dutch_region";
	mappers::CultureMappingRule theMapping(input);
	theMapping.insertRegionMapper(theMapper);

	ASSERT_FALSE(theMapping.cultureMatch("german", "", 17, ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnRegionFailsForNoRegion)
{
	auto theMapper = std::make_shared<mappers::RegionMapper>();
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }";
	theMapper->loadRegions(areaStream, regionStream, superRegionStream);

	std::stringstream input;
	input << "eu4 = dutch ck3 = german region = dutch_region";
	mappers::CultureMappingRule theMapping(input);
	theMapping.insertRegionMapper(theMapper);

	ASSERT_FALSE(theMapping.cultureMatch("german", "", 17, ""));
}

TEST(Mappers_CultureMappingRuleTests, regularMatchOnRegionFailsForNoProvince)
{
	auto theMapper = std::make_shared<mappers::RegionMapper>();
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }";
	theMapper->loadRegions(areaStream, regionStream, superRegionStream);

	std::stringstream input;
	input << "eu4 = dutch ck3 = german region = dutch_region";
	mappers::CultureMappingRule theMapping(input);
	theMapping.insertRegionMapper(theMapper);

	ASSERT_FALSE(theMapping.cultureMatch("german", "", 0, ""));
}

TEST(Mappers_CultureMappingRuleTests, regionalMatchOnRegion)
{
	auto theMapper = std::make_shared<mappers::RegionMapper>();
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } ";
	std::stringstream regionStream;
	regionStream << "dutch_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { dutch_region }";
	theMapper->loadRegions(areaStream, regionStream, superRegionStream);

	std::stringstream input;
	input << "eu4 = dutch ck3 = german region = dutch_region region = atlantic_sea";
	mappers::CultureMappingRule theMapping(input);
	theMapping.insertRegionMapper(theMapper);

	ASSERT_EQ("dutch", *theMapping.cultureMatch("german", "", 1, ""));
}

TEST(Mappers_CultureMappingRuleTests, regionalMatchOnFailsForNoLoadedRegions)
{
	std::stringstream input;
	input << "eu4 = dutch ck3 = german";
	const mappers::CultureMappingRule theMapping(input);

	ASSERT_FALSE(theMapping.cultureRegionalMatch("german", "", 1, ""));
}

TEST(Mappers_CultureMappingRuleTests, nonRegionalNonReligionsMatch)
{
	std::stringstream input;
	input << "eu4 = dutch ck3 = german";
	const mappers::CultureMappingRule theMapping(input);

	ASSERT_EQ("dutch", *theMapping.cultureNonRegionalNonReligiousMatch("german", "", 1, ""));
}

TEST(Mappers_CultureMappingRuleTests, nonRegionalNonReligionsMatchOnFailsForLoadedRegions)
{
	std::stringstream input;
	input << "eu4 = dutch ck3 = german region = test_region";
	const mappers::CultureMappingRule theMapping(input);

	ASSERT_FALSE(theMapping.cultureNonRegionalNonReligiousMatch("german", "", 1, ""));
}

TEST(Mappers_CultureMappingRuleTests, nonRegionalNonReligionsMatchOnFailsForLoadedReligions)
{
	std::stringstream input;
	input << "eu4 = dutch ck3 = german religion = test_religion";
	const mappers::CultureMappingRule theMapping(input);

	ASSERT_FALSE(theMapping.cultureNonRegionalNonReligiousMatch("german", "", 1, ""));
}

TEST(Mappers_CultureMappingRuleTests, TechGroupCanBeMatched)
{
	std::stringstream input;
	input << "= { eu4 = aztec tech = high_american }";

	const mappers::CultureMappingRule theMapping(input);

	ASSERT_EQ("high_american", *theMapping.getTechGroup("aztec"));
}

TEST(Mappers_CultureMappingRuleTests, TechGroupFailsForWrongMatch)
{
	std::stringstream input;
	input << "= { eu4 = aztec tech = high_american }";

	const mappers::CultureMappingRule theMapping(input);

	ASSERT_FALSE(theMapping.getTechGroup("incan"));
}

TEST(Mappers_CultureMappingRuleTests, GFXCanBeMatched)
{
	std::stringstream input;
	input << "= { eu4 = aztec gfx = test }";

	const mappers::CultureMappingRule theMapping(input);

	ASSERT_EQ("test", *theMapping.getGFX("aztec"));
}

TEST(Mappers_CultureMappingRuleTests, GFXFailsForWrongMatch)
{
	std::stringstream input;
	input << "= { eu4 = aztec gfx = test }";

	const mappers::CultureMappingRule theMapping(input);

	ASSERT_FALSE(theMapping.getGFX("incan"));
}
