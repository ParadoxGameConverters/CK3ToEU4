#include "../CK3ToEU4/Source/CK3World/Geography/CountyDetails.h"
#include "../CK3ToEU4/Source/CK3World/Titles/LandedTitles.h"
#include "../CK3ToEU4/Source/CK3World/Titles/Title.h"
#include "../CK3ToEU4/Source/CK3World/Titles/Titles.h"
#include "../CK3ToEU4/Source/Mappers/ProvinceMapper/ProvinceMapper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_ProvinceMapperTests, canLookupCK3Titles)
{
	std::stringstream provinceMapperStream;
	provinceMapperStream << "0.0.0.0 = {\n";
	provinceMapperStream << "	link = { eu4 = 2 eu4 = 1 ck3 = 2 ck3 = 1 }\n";
	provinceMapperStream << "}";
	mappers::ProvinceMapper provinceMapper(provinceMapperStream);

	std::stringstream countyDetailsStream;
	countyDetailsStream << "c_county1 = {faith = 4}\n";
	countyDetailsStream << "c_county2 = { development = 8 }\n";
	CK3::CountyDetails countyDetails(countyDetailsStream);

	std::stringstream landedTitlesStream;
	landedTitlesStream << "b_barony1={province=1}\n";
	landedTitlesStream << "b_barony2={province=2}\n";
	landedTitlesStream << "c_county1={}\n";
	landedTitlesStream << "c_county2={}\n";
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedTitlesStream);
	landedTitles.linkCountyDetails(countyDetails);

	std::stringstream titlesStream;
	titlesStream << "1={key=b_barony1 name=Barony1 de_jure_liege=3}\n";
	titlesStream << "2={key=b_barony2 name=Barony2 de_jure_liege=4}\n";
	titlesStream << "3={key=c_county1 name=County1}\n";
	titlesStream << "4={key=c_county2 name=County2}\n";
	CK3::Titles titles(titlesStream);
	titles.linkTitles();
	titles.linkLandedTitles(landedTitles);
	provinceMapper.transliterateMappings(titles.getTitles());

	ASSERT_EQ(2, provinceMapper.getCK3Titles(1).size());
	ASSERT_EQ("County1", provinceMapper.getCK3Titles(1).find("c_county1")->second->getDisplayName());
	ASSERT_EQ("County2", provinceMapper.getCK3Titles(1).find("c_county2")->second->getDisplayName());
}

TEST(Mappers_ProvinceMapperTests, canLookupCK3TitlesAmongMultipleRules)
{
	std::stringstream provinceMapperStream;
	provinceMapperStream << "0.0.0.0 = {\n";
	provinceMapperStream << "	link = { eu4 = 1 ck3 = 1}\n";
	provinceMapperStream << "	link = { eu4 = 2 ck3 = 2}\n";
	provinceMapperStream << "}";
	mappers::ProvinceMapper provinceMapper(provinceMapperStream);

	std::stringstream countyDetailsStream;
	countyDetailsStream << "c_county1 = {faith = 4}\n";
	countyDetailsStream << "c_county2 = { development = 8 }\n";
	CK3::CountyDetails countyDetails(countyDetailsStream);

	std::stringstream landedTitlesStream;
	landedTitlesStream << "b_barony1={province=1}\n";
	landedTitlesStream << "b_barony2={province=2}\n";
	landedTitlesStream << "c_county1={}\n";
	landedTitlesStream << "c_county2={}\n";
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedTitlesStream);
	landedTitles.linkCountyDetails(countyDetails);

	std::stringstream titlesStream;
	titlesStream << "1={key=b_barony1 name=Barony1 de_jure_liege=3}\n";
	titlesStream << "2={key=b_barony2 name=Barony2 de_jure_liege=4}\n";
	titlesStream << "3={key=c_county1 name=County1}\n";
	titlesStream << "4={key=c_county2 name=County2}\n";
	CK3::Titles titles(titlesStream);
	titles.linkTitles();
	titles.linkLandedTitles(landedTitles);
	provinceMapper.transliterateMappings(titles.getTitles());

	ASSERT_EQ(1, provinceMapper.getCK3Titles(1).size());
	ASSERT_EQ("County1", provinceMapper.getCK3Titles(1).find("c_county1")->second->getDisplayName());
	ASSERT_EQ(1, provinceMapper.getCK3Titles(2).size());
	ASSERT_EQ("County2", provinceMapper.getCK3Titles(2).find("c_county2")->second->getDisplayName());
}

TEST(Mappers_ProvinceMapperTests, provinceMapperTransliterationDoesNothingForClaylessBaronies)
{
	std::stringstream provinceMapperStream;
	provinceMapperStream << "0.0.0.0 = {\n";
	provinceMapperStream << "	link = { eu4 = 1 ck3 = 1}\n";
	provinceMapperStream << "	link = { eu4 = 2 ck3 = 2}\n";
	provinceMapperStream << "}";
	mappers::ProvinceMapper provinceMapper(provinceMapperStream);

	std::stringstream countyDetailsStream;
	countyDetailsStream << "c_county1 = {faith = 4}\n";
	countyDetailsStream << "c_county2 = { development = 8 }\n";
	CK3::CountyDetails countyDetails(countyDetailsStream);

	std::stringstream landedTitlesStream;
	landedTitlesStream << "b_barony1={province=1}\n";
	landedTitlesStream << "b_barony2={}\n"; // <- no province data
	landedTitlesStream << "c_county1={}\n";
	landedTitlesStream << "c_county2={}\n";
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedTitlesStream);
	landedTitles.linkCountyDetails(countyDetails);

	std::stringstream titlesStream;
	titlesStream << "1={key=b_barony1 name=Barony1 de_jure_liege=3}\n";
	titlesStream << "2={key=b_barony2 name=Barony2 de_jure_liege=4}\n";
	titlesStream << "3={key=c_county1 name=County1}\n";
	titlesStream << "4={key=c_county2 name=County2}\n";
	CK3::Titles titles(titlesStream);
	titles.linkTitles();
	titles.linkLandedTitles(landedTitles);

	provinceMapper.transliterateMappings(titles.getTitles());

	ASSERT_EQ(1, provinceMapper.getCK3Titles(1).size());
	ASSERT_EQ("County1", provinceMapper.getCK3Titles(1).find("c_county1")->second->getDisplayName());
	ASSERT_TRUE(provinceMapper.getCK3Titles(2).empty());
}

TEST(Mappers_ProvinceMapperTests, provinceMapperTransliterationdoesNothingForDejureLessBaronies)
{
	std::stringstream provinceMapperStream;
	provinceMapperStream << "0.0.0.0 = {\n";
	provinceMapperStream << "	link = { eu4 = 1 ck3 = 1}\n";
	provinceMapperStream << "	link = { eu4 = 2 ck3 = 2}\n";
	provinceMapperStream << "}";
	mappers::ProvinceMapper provinceMapper(provinceMapperStream);

	std::stringstream countyDetailsStream;
	countyDetailsStream << "c_county1 = {faith = 4}\n";
	countyDetailsStream << "c_county2 = { development = 8 }\n";
	CK3::CountyDetails countyDetails(countyDetailsStream);

	std::stringstream landedTitlesStream;
	landedTitlesStream << "b_barony1={province=1}\n";
	landedTitlesStream << "b_barony2={province=2}\n";
	landedTitlesStream << "c_county1={}\n";
	landedTitlesStream << "c_county2={}\n";
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedTitlesStream);
	landedTitles.linkCountyDetails(countyDetails);

	std::stringstream titlesStream;
	titlesStream << "1={key=b_barony1 name=Barony1 de_jure_liege=3}\n";
	titlesStream << "2={key=b_barony2 name=Barony2}\n"; // <- no dejure
	titlesStream << "3={key=c_county1 name=County1}\n";
	titlesStream << "4={key=c_county2 name=County2}\n";
	CK3::Titles titles(titlesStream);
	titles.linkTitles();
	titles.linkLandedTitles(landedTitles);

	provinceMapper.transliterateMappings(titles.getTitles());

	ASSERT_EQ(1, provinceMapper.getCK3Titles(1).size());
	ASSERT_EQ("County1", provinceMapper.getCK3Titles(1).find("c_county1")->second->getDisplayName());
	ASSERT_TRUE(provinceMapper.getCK3Titles(2).empty());
}

TEST(Mappers_ProvinceMapperTests, provinceMapperTransliterationDoesNothingForInsaneCounties)
{
	std::stringstream provinceMapperStream;
	provinceMapperStream << "0.0.0.0 = {\n";
	provinceMapperStream << "	link = { eu4 = 1 ck3 = 1 }\n";
	provinceMapperStream << "	link = { eu4 = 2 ck3 = 2 }\n";
	provinceMapperStream << "}";
	mappers::ProvinceMapper provinceMapper(provinceMapperStream);

	std::stringstream countyDetailsStream;
	countyDetailsStream << "c_county1 = {faith = 4}\n";
	countyDetailsStream << "c_county2 = { development = 8 }\n";
	CK3::CountyDetails countyDetails(countyDetailsStream);

	std::stringstream landedTitlesStream;
	landedTitlesStream << "b_barony1={province=1}\n";
	landedTitlesStream << "b_barony2={province=2}\n";
	landedTitlesStream << "c_county1={}\n"; // <- no county2 landed data
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedTitlesStream);
	landedTitles.linkCountyDetails(countyDetails);

	std::stringstream titlesStream;
	titlesStream << "1={key=b_barony1 name=Barony1 de_jure_liege=3}\n";
	titlesStream << "2={key=b_barony2 name=Barony2 de_jure_liege=4}\n";
	titlesStream << "3={key=c_county1 name=County1}\n";
	titlesStream << "4={key=c_county2 name=County2}\n";
	CK3::Titles titles(titlesStream);
	titles.linkTitles();
	titles.linkLandedTitles(landedTitles);

	provinceMapper.transliterateMappings(titles.getTitles());

	ASSERT_EQ(1, provinceMapper.getCK3Titles(1).size());
	ASSERT_EQ("County1", provinceMapper.getCK3Titles(1).find("c_county1")->second->getDisplayName());
	ASSERT_TRUE(provinceMapper.getCK3Titles(2).empty());
}

TEST(Mappers_ProvinceMapperTests, provinceMapperTransliterationDoesNothingForInvalidMapping)
{
	std::stringstream provinceMapperStream;
	provinceMapperStream << "0.0.0.0 = {\n";
	provinceMapperStream << "	link = { eu4 = 1 ck3 = 1 }\n";
	provinceMapperStream << "	link = { eu4 = 2 ck3 = 3 }\n"; // <- invalid ck3 = 3 mapping
	provinceMapperStream << "}";
	mappers::ProvinceMapper provinceMapper(provinceMapperStream);

	std::stringstream countyDetailsStream;
	countyDetailsStream << "c_county1 = {faith = 4}\n";
	countyDetailsStream << "c_county2 = { development = 8 }\n";
	CK3::CountyDetails countyDetails(countyDetailsStream);

	std::stringstream landedTitlesStream;
	landedTitlesStream << "b_barony1={province=1}\n";
	landedTitlesStream << "b_barony2={province=2}\n";
	landedTitlesStream << "c_county1={}\n";
	landedTitlesStream << "c_county2={}\n";
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedTitlesStream);
	landedTitles.linkCountyDetails(countyDetails);

	std::stringstream titlesStream;
	titlesStream << "1={key=b_barony1 name=Barony1 de_jure_liege=3}\n";
	titlesStream << "2={key=b_barony2 name=Barony2 de_jure_liege=4}\n";
	titlesStream << "3={key=c_county1 name=County1}\n";
	titlesStream << "4={key=c_county2 name=County2}\n";
	CK3::Titles titles(titlesStream);
	titles.linkTitles();
	titles.linkLandedTitles(landedTitles);

	provinceMapper.transliterateMappings(titles.getTitles());

	ASSERT_EQ(1, provinceMapper.getCK3Titles(1).size());
	ASSERT_EQ("County1", provinceMapper.getCK3Titles(1).find("c_county1")->second->getDisplayName());
	ASSERT_TRUE(provinceMapper.getCK3Titles(2).empty());
}

TEST(Mappers_ProvinceMapperTests, canLookupEU4Provinces)
{
	std::stringstream provinceMapperStream;
	provinceMapperStream << "0.0.0.0 = {\n";
	provinceMapperStream << "	link = { eu4 = 2 eu4 = 1 ck3 = 2 ck3 = 1 }\n";
	provinceMapperStream << "}";
	mappers::ProvinceMapper provinceMapper(provinceMapperStream);

	std::stringstream countyDetailsStream;
	countyDetailsStream << "c_county1 = {faith = 4}\n";
	countyDetailsStream << "c_county2 = { development = 8 }\n";
	CK3::CountyDetails countyDetails(countyDetailsStream);

	std::stringstream landedTitlesStream;
	landedTitlesStream << "b_barony1={province=1}\n";
	landedTitlesStream << "b_barony2={province=2}\n";
	landedTitlesStream << "c_county1={}\n";
	landedTitlesStream << "c_county2={}\n";
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedTitlesStream);
	landedTitles.linkCountyDetails(countyDetails);

	std::stringstream titlesStream;
	titlesStream << "1={key=b_barony1 name=Barony1 de_jure_liege=3}\n";
	titlesStream << "2={key=b_barony2 name=Barony2 de_jure_liege=4}\n";
	titlesStream << "3={key=c_county1 name=County1}\n";
	titlesStream << "4={key=c_county2 name=County2}\n";
	CK3::Titles titles(titlesStream);
	titles.linkTitles();
	titles.linkLandedTitles(landedTitles);
	provinceMapper.transliterateMappings(titles.getTitles());

	// Grab a title to match against mapper.
	auto county2 = titles.getTitles().find("c_county2");

	ASSERT_EQ(2, provinceMapper.getEU4ProvinceNumbers(county2->first).size());
	ASSERT_EQ(2, provinceMapper.getEU4ProvinceNumbers(county2->first)[0]);
	ASSERT_EQ(1, provinceMapper.getEU4ProvinceNumbers(county2->first)[1]);
}

TEST(Mappers_ProvinceMapperTests, EU4ProvinceLookupReturnsNothingForInvalidTitleName)
{
	std::stringstream provinceMapperStream;
	provinceMapperStream << "0.0.0.0 = {\n";
	provinceMapperStream << "	link = { eu4 = 2 eu4 = 1 ck3 = 2 ck3 = 1 }\n";
	provinceMapperStream << "}";
	mappers::ProvinceMapper provinceMapper(provinceMapperStream);

	std::stringstream countyDetailsStream;
	countyDetailsStream << "c_county1 = {faith = 4}\n";
	countyDetailsStream << "c_county2 = { development = 8 }\n";
	CK3::CountyDetails countyDetails(countyDetailsStream);

	std::stringstream landedTitlesStream;
	landedTitlesStream << "b_barony1={province=1}\n";
	landedTitlesStream << "b_barony2={province=2}\n";
	landedTitlesStream << "c_county1={}\n";
	landedTitlesStream << "c_county2={}\n";
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedTitlesStream);
	landedTitles.linkCountyDetails(countyDetails);

	std::stringstream titlesStream;
	titlesStream << "1={key=b_barony1 name=Barony1 de_jure_liege=3}\n";
	titlesStream << "2={key=b_barony2 name=Barony2 de_jure_liege=4}\n";
	titlesStream << "3={key=c_county1 name=County1}\n";
	titlesStream << "4={key=c_county2 name=County2}\n";
	CK3::Titles titles(titlesStream);
	titles.linkTitles();
	titles.linkLandedTitles(landedTitles);
	provinceMapper.transliterateMappings(titles.getTitles());

	ASSERT_TRUE(provinceMapper.getEU4ProvinceNumbers("nonsense").empty());
}

TEST(Mappers_ProvinceMapperTests, CK3TitleLookupReturnsNothingForInvalidEU4ProvinceID)
{
	std::stringstream provinceMapperStream;
	provinceMapperStream << "0.0.0.0 = {\n";
	provinceMapperStream << "	link = { eu4 = 2 eu4 = 1 ck3 = 2 ck3 = 1 }\n";
	provinceMapperStream << "}";
	mappers::ProvinceMapper provinceMapper(provinceMapperStream);

	std::stringstream countyDetailsStream;
	countyDetailsStream << "c_county1 = {faith = 4}\n";
	countyDetailsStream << "c_county2 = { development = 8 }\n";
	CK3::CountyDetails countyDetails(countyDetailsStream);

	std::stringstream landedTitlesStream;
	landedTitlesStream << "b_barony1={province=1}\n";
	landedTitlesStream << "b_barony2={province=2}\n";
	landedTitlesStream << "c_county1={}\n";
	landedTitlesStream << "c_county2={}\n";
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedTitlesStream);
	landedTitles.linkCountyDetails(countyDetails);

	std::stringstream titlesStream;
	titlesStream << "1={key=b_barony1 name=Barony1 de_jure_liege=3}\n";
	titlesStream << "2={key=b_barony2 name=Barony2 de_jure_liege=4}\n";
	titlesStream << "3={key=c_county1 name=County1}\n";
	titlesStream << "4={key=c_county2 name=County2}\n";
	CK3::Titles titles(titlesStream);
	titles.linkTitles();
	titles.linkLandedTitles(landedTitles);
	provinceMapper.transliterateMappings(titles.getTitles());

	ASSERT_TRUE(provinceMapper.getCK3Titles(99).empty());
}
