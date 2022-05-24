#include "../CK3toEU4/Source/CK3World/Geography/CountyDetails.h"
#include "../CK3toEU4/Source/CK3World/Titles/LandedTitles.h"
#include "../CK3toEU4/Source/CK3World/Titles/Title.h"
#include "../CK3toEU4/Source/CK3World/Titles/Titles.h"
#include "../CK3toEU4/Source/EU4World/Province/EU4Province.h"
#include "../CK3toEU4/Source/Mappers/CultureMapper/CultureMapper.h"
#include "../CK3toEU4/Source/Mappers/LocDegraderMapper/LocDegraderMapper.h"
#include "../CK3toEU4/Source/Mappers/ProvinceMapper/ProvinceMapper.h"
#include "../CK3toEU4/Source/Mappers/ReligionMapper/ReligionMapper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_NameTransferTests, oneToOneTransfersName)
{
	std::stringstream provinceMapperStream;
	provinceMapperStream << "0.0.0.0 = {\n";
	provinceMapperStream << " link = { eu4 = 1 ck3 = 1 }\n";
	provinceMapperStream << "}";
	mappers::ProvinceMapper provinceMapper(provinceMapperStream);

	std::stringstream landedTitlesStream;
	landedTitlesStream << "b_barony1 = { province = 1 }\n";
	landedTitlesStream << "c_county1 = { }\n";
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedTitlesStream);
	// landedTitles.linkCountyDetails(countyDetails); May need Development values in order to work.

	std::stringstream titlesStream;
	titlesStream << "1={key=b_barony1 name=Barony1 de_jure_liege=2}\n";
	titlesStream << "2={key=c_county1 name=County1 renamed=yes}\n";
	CK3::Titles titles(titlesStream);
	titles.linkTitles();
	titles.linkLandedTitles(landedTitles);

	provinceMapper.transliterateMappings(titles.getTitles());

	// Bypassing determineProvinceSource, which does not have a test
	const auto& srcProvince = titles.getTitles().at("c_county1");
	srcProvince->pickDisplayName(titles.getTitles());

	// initialize an EU4 province with initializeFromCK3Title
	EU4::Province outProvince = EU4::Province();
	std::stringstream dummy;
	dummy << "link = { eu4 = culture ck3 = culture }";
	const mappers::CultureMapper culMap(dummy);
	const mappers::ReligionMapper religMap(dummy);
	const mappers::LocDegraderMapper locDegrader(dummy);

	// Fatal error LNK2019: unresolved external symbol 
	outProvince.initializeFromCK3Title(srcProvince, culMap, religMap, locDegrader); // gives error LNK2019
	outProvince.setAdm(5); // works
	outProvince.cul(culMap); // gives error LNK2019

	EXPECT_EQ(outProvince.isRenamed(), true);				// EU4 srcProvince should have the renamed flag
	EXPECT_EQ(outProvince.getCustomName(), "County1"); // EU4 province should have the new manual displayName
}
// Things to test
// 1:1
// N:1
// 1:M
// N:M
// non-latin base not carried forward
// duchy capital priority observed
// duchy capital priority not observed when duchy owner split
// duchy capital priority not observed when in separate mapping