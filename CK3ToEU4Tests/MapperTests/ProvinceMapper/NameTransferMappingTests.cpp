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
	landedTitlesStream << "c_county1 = {\n";
	landedTitlesStream << "\tb_barony1 = { province = 1 }\n";
	landedTitlesStream << "}\n";
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedTitlesStream);

	std::stringstream titlesStream;
	titlesStream << "1={key=c_county1 name=County1 renamed=yes de_jure_vassals={2}}\n";
	titlesStream << "2={key=b_barony1 name=Barony1 de_facto_liege=1 de_jure_liege=1}\n";
	CK3::Titles titles(titlesStream);
	titles.linkTitles();
	titles.linkLandedTitles(landedTitles);

	provinceMapper.transliterateMappings(titles.getTitles());

	// Bypassing determineProvinceSource, which does not have a test
	const auto& srcProvince = titles.getTitles().at("c_county1");
	srcProvince->pickDisplayName(titles.getTitles());

	// initialize an EU4 province
	EU4::Province outProvince = EU4::Province(srcProvince);
	std::stringstream dummy;
	const mappers::LocDegraderMapper locDegrader(dummy);

	outProvince.registerManualName(locDegrader);

	EXPECT_EQ(outProvince.isRenamed(), true);				// EU4 province should have the renamed flag
	EXPECT_EQ(outProvince.getCustomName(), "County1"); // EU4 province should have the new manual displayName
}
/*TEST(Mappers_NameTransferTests, manyToOneTransfersName)
{
	// Requires a call to EU4::World::determineProvinceSource which needs a whole lot more info set up
	std::stringstream provinceMapperStream;
	provinceMapperStream << "0.0.0.0 = {\n";
	provinceMapperStream << " link = { eu4 = 1 ck3 = 1 ck3 = 2 }\n";
	provinceMapperStream << "}";
	mappers::ProvinceMapper provinceMapper(provinceMapperStream);

	std::stringstream landedTitlesStream;
	landedTitlesStream << "c_county1 = {\n";
	landedTitlesStream << "\tb_barony1 = { province = 1 }\n";
	landedTitlesStream << "}\n";
	landedTitlesStream << "c_county2 = {\n";
	landedTitlesStream << "\tb_barony2 = { province = 2 }\n";
	landedTitlesStream << "}\n";
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedTitlesStream);

	std::stringstream titlesStream;
	titlesStream << "1={key=c_county1 name=County1 renamed=yes de_jure_vassals={2}}\n";
	titlesStream << "2={key=b_barony1 name=Barony1 de_facto_liege=1 de_jure_liege=1}\n";
	titlesStream << "3={key=c_county2 name=County2 renamed=yes de_jure_vassals={4}}\n";
	titlesStream << "4={key=b_barony2 name=Barony2 de_facto_liege=3 de_jure_liege=3}\n";
	CK3::Titles titles(titlesStream);
	titles.linkTitles();
	titles.linkLandedTitles(landedTitles);

	provinceMapper.transliterateMappings(titles.getTitles());

	std::stringstream countyDetailsStream;
	countyDetailsStream << "c_county1 = { development = 1 }\n";
	countyDetailsStream << "c_county2 = { development = 2 }\n";
	CK3::CountyDetails countyDetails(countyDetailsStream);
	landedTitles.linkCountyDetails(countyDetails);

	// A call to determineProvinceSource

	// initialize an EU4 province
	EU4::Province outProvince = EU4::Province(srcProvince);
	std::stringstream dummy;
	const mappers::LocDegraderMapper locDegrader(dummy);

	outProvince.registerManualName(locDegrader);

	EXPECT_EQ(outProvince.isRenamed(), true);				// EU4 province should have the renamed flag
	EXPECT_EQ(outProvince.getCustomName(), "County2"); // EU4 province should have name from the province specified in determineProvinceSource
}*/
TEST(Mappers_NameTransferTests, oneToManyTransfersName)
{
	std::stringstream provinceMapperStream;
	provinceMapperStream << "0.0.0.0 = {\n";
	provinceMapperStream << " link = { eu4 = 1 eu4 = 2 ck3 = 1 }\n";
	provinceMapperStream << "}";
	mappers::ProvinceMapper provinceMapper(provinceMapperStream);

	std::stringstream landedTitlesStream;
	landedTitlesStream << "c_county1 = {\n";
	landedTitlesStream << "\tb_barony1 = { province = 1 }\n";
	landedTitlesStream << "}\n";
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedTitlesStream);

	std::stringstream titlesStream;
	titlesStream << "1={key=c_county1 name=County1 renamed=yes de_jure_vassals={2}}\n";
	titlesStream << "2={key=b_barony1 name=Barony1 de_facto_liege=1 de_jure_liege=1}\n";
	CK3::Titles titles(titlesStream);
	titles.linkTitles();
	titles.linkLandedTitles(landedTitles);

	provinceMapper.transliterateMappings(titles.getTitles());


	// Bypassing determineProvinceSource, which does not have a test
	const auto& srcProvince = titles.getTitles().at("c_county1");
	srcProvince->pickDisplayName(titles.getTitles());

	// initialize an EU4 province
	EU4::Province outProvince = EU4::Province(srcProvince);
	EU4::Province outProvince2 = EU4::Province(srcProvince);
	std::stringstream dummy;
	const mappers::LocDegraderMapper locDegrader(dummy);

	outProvince.registerManualName(locDegrader);
	outProvince2.registerManualName(locDegrader);

	EXPECT_EQ(outProvince.isRenamed(), true);				// EU4 province should have the renamed flag
	EXPECT_EQ(outProvince.getCustomName(), "County1"); // EU4 province should have the new manual displayName
	EXPECT_EQ(outProvince2.isRenamed(), false);			// second EU4 province should NOT have the renamed flag
}
TEST(Mappers_NameTransferTests, nondegradeableNamesDefualtsToVanilla)
{
	std::stringstream provinceMapperStream;
	provinceMapperStream << "0.0.0.0 = {\n";
	provinceMapperStream << " link = { eu4 = 1 ck3 = 1 }\n";
	provinceMapperStream << "}";
	mappers::ProvinceMapper provinceMapper(provinceMapperStream);

	std::stringstream landedTitlesStream;
	landedTitlesStream << "c_county1 = {\n";
	landedTitlesStream << "\tb_barony1 = { province = 1 }\n";
	landedTitlesStream << "}\n";
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedTitlesStream);

	std::stringstream titlesStream;
	titlesStream << "1={key=c_county1 name=Cøunty1 renamed=yes de_jure_vassals={2}}\n"; // Notice the ø
	titlesStream << "2={key=b_barony1 name=Barony1 de_facto_liege=1 de_jure_liege=1}\n";
	CK3::Titles titles(titlesStream);
	titles.linkTitles();
	titles.linkLandedTitles(landedTitles);

	provinceMapper.transliterateMappings(titles.getTitles());

	// Bypassing determineProvinceSource, which does not have a test
	const auto& srcProvince = titles.getTitles().at("c_county1");
	srcProvince->pickDisplayName(titles.getTitles());

	// initialize an EU4 province
	EU4::Province outProvince = EU4::Province(srcProvince);
	std::stringstream dummy;
	const mappers::LocDegraderMapper locDegrader(dummy);

	outProvince.registerManualName(locDegrader);

	EXPECT_EQ(outProvince.isRenamed(), false); // EU4 province should NOT have the renamed flag
}

// Can't test without creating a skeleton EU4::World
// N:1
// duchy capital priority observed
// duchy capital priority not observed when duchy owner split
// duchy capital priority not observed when in separate mapping