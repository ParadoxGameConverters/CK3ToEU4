#include "../../CK3ToEU4/Source/CK3World/Geography/CountyDetail.h"
#include "../../CK3ToEU4/Source/CK3World/Geography/CountyDetails.h"
#include "../../CK3ToEU4/Source/CK3World/Geography/ProvinceHolding.h"
#include "../../CK3ToEU4/Source/CK3World/Geography/ProvinceHoldings.h"
#include "../../CK3ToEU4/Source/CK3World/Titles/LandedTitles.h"
#include "../../CK3ToEU4/Source/CK3World/Titles/Title.h"
#include "../../CK3ToEU4/Source/CK3World/Titles/Titles.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_LandedTitlesTests, titlePrimitivesDefaultToBlank)
{
	std::stringstream input;
	CK3::LandedTitles titles;
	titles.loadTitles(input);

	ASSERT_FALSE(titles.isDefiniteForm());
	ASSERT_FALSE(titles.isLandless());
	ASSERT_FALSE(titles.getColor());
	ASSERT_FALSE(titles.getCapital());
	ASSERT_FALSE(titles.getProvince());
}

TEST(CK3World_LandedTitlesTests, titlePrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "definite_form = yes\n";
	input << "landless = yes\n";
	input << "color = { 23 23 23 }\n";
	input << "capital = c_roma\n";
	input << "province = 345\n";

	CK3::LandedTitles titles;
	titles.loadTitles(input);

	ASSERT_TRUE(titles.isDefiniteForm());
	ASSERT_TRUE(titles.isLandless());
	ASSERT_EQ("= rgb { 23 23 23 }", titles.getColor()->outputRgb());
	ASSERT_EQ("c_roma", titles.getCapital()->first);
	ASSERT_EQ(345, titles.getProvince()->first);
}

TEST(CK3World_LandedTitlesTests, titlesDefaultToEmpty)
{
	std::stringstream input;
	CK3::LandedTitles titles;
	titles.loadTitles(input);

	ASSERT_TRUE(titles.getFoundTitles().empty());
}

TEST(CK3World_LandedTitlesTests, titlesCanBeLoaded)
{
	std::stringstream input;
	input << "b_barony = { province = 12 }\n";
	input << "c_county = { landless = yes }\n";

	CK3::LandedTitles titles;
	titles.loadTitles(input);

	const auto& barony = titles.getFoundTitles().find("b_barony");
	const auto& county = titles.getFoundTitles().find("c_county");

	ASSERT_EQ(2, titles.getFoundTitles().size());
	ASSERT_EQ(12, barony->second->getProvince()->first);
	ASSERT_TRUE(county->second->isLandless());
}

TEST(CK3World_LandedTitlesTests, titlesCanBeLoadedRecursively)
{
	std::stringstream input;
	input << "e_empire1 = { k_kingdom2 = { d_duchy3 = { b_barony4 = { province = 12 } } } }\n";
	input << "c_county5 = { landless = yes }\n";

	CK3::LandedTitles titles;
	titles.loadTitles(input);

	const auto& barony = titles.getFoundTitles().find("b_barony4");
	const auto& county = titles.getFoundTitles().find("c_county5");

	ASSERT_EQ(5, titles.getFoundTitles().size());
	ASSERT_EQ(12, barony->second->getProvince()->first);
	ASSERT_TRUE(county->second->isLandless());
}

TEST(CK3World_LandedTitlesTests, titlesCanBeOverriddenByMods)
{
	std::stringstream input;
	input << "e_empire1 = { k_kingdom2 = { d_duchy3 = { b_barony4 = { province = 12 } } } }\n";
	input << "c_county5 = { landless = yes }\n";

	CK3::LandedTitles titles;
	titles.loadTitles(input);

	std::stringstream input2;
	input2 << "b_barony4 = { province = 15 }\n";
	input2 << "c_county5 = { landless = NO }\n";
	titles.loadTitles(input2);

	const auto& barony = titles.getFoundTitles().find("b_barony4");
	const auto& county = titles.getFoundTitles().find("c_county5");

	ASSERT_EQ(5, titles.getFoundTitles().size());
	ASSERT_EQ(15, barony->second->getProvince()->first);
	ASSERT_FALSE(county->second->isLandless());
}

TEST(CK3World_LandedTitlesTests, titlesCanBeAddedByMods)
{
	std::stringstream input;
	input << "e_empire1 = { k_kingdom2 = { d_duchy3 = { b_barony4 = { province = 12 } } } }\n";
	input << "c_county5 = { landless = yes }\n";

	CK3::LandedTitles titles;
	titles.loadTitles(input);

	std::stringstream input2;
	input2 << "c_county5 = { landless = NO }\n"; // Overrides existing instance
	input2 << "e_empire6 = { k_kingdom7 = { d_duchy8 = { b_barony9 = { province = 12 } } } }\n";
	input2 << "c_county10 = { landless = yes }\n";
	titles.loadTitles(input2);

	ASSERT_EQ(10, titles.getFoundTitles().size());
}

TEST(CK3World_LandedTitlesTests, holdingsCanBeLinked)
{
	std::stringstream input;
	input << "e_empire1 = { k_kingdom2 = { d_duchy3 = { b_barony4 = { province = 12 } } } }\n";
	input << "b_barony2 = { province = 13 }\n";
	CK3::LandedTitles titles;
	titles.loadTitles(input);

	std::stringstream input2;
	input2 << "12={holding={type=\"city_holding\"\n}}\n";
	input2 << "13={holding={type=\"castle_holding\"\nbuildings={ {type=\"hunting_grounds_01\"} {} {} {type=\"hill_farms_02\"} {} {}}}\n";
	const CK3::ProvinceHoldings provinces(input2);

	titles.linkProvinceHoldings(provinces);
	const auto& b1 = titles.getFoundTitles().find("b_barony4");
	const auto& b2 = titles.getFoundTitles().find("b_barony2");

	ASSERT_EQ("city_holding", b1->second->getProvince()->second->getHoldingType());
	ASSERT_EQ("castle_holding", b2->second->getProvince()->second->getHoldingType());
}

TEST(CK3World_LandedTitlesTests, missingHoldingsLinkThrowsException)
{
	std::stringstream input;
	input << "e_empire1 = { k_kingdom2 = { d_duchy3 = { b_barony4 = { province = 12 } } } }\n";
	input << "b_barony2 = { province = 13 }\n";
	CK3::LandedTitles titles;
	titles.loadTitles(input);

	std::stringstream input2;
	input << "12={holding={type=\"city_holding\"\n}}\n";
	const CK3::ProvinceHoldings provinces(input);

	ASSERT_THROW(titles.linkProvinceHoldings(provinces), std::runtime_error);
}

TEST(CK3World_LandedTitlesTests, missingProvinceLinkThrowsException)
{
	std::stringstream input;
	input << "e_empire1 = { k_kingdom2 = { d_duchy3 = { b_barony4 = { } } } }\n"; // missing province
	input << "b_barony2 = { province = 13 }\n";
	CK3::LandedTitles titles;
	titles.loadTitles(input);

	std::stringstream input2;
	input2 << "12={holding={type=\"city_holding\"\n}}\n";
	input2 << "13={holding={type=\"castle_holding\"\nbuildings={ {type=\"hunting_grounds_01\"} {} {} {type=\"hill_farms_02\"} {} {}}}\n";
	const CK3::ProvinceHoldings provinces(input2);

	ASSERT_THROW(titles.linkProvinceHoldings(provinces), std::runtime_error);
}

TEST(CK3World_LandedTitlesTests, countiesCanBeLinked)
{
	std::stringstream input;
	input << "e_empire1 = { k_kingdom2 = { c_county3 = { b_barony4 = { province = 12 } } } }\n";
	input << "c_county5 = { }\n";
	CK3::LandedTitles titles;
	titles.loadTitles(input);

	std::stringstream input2;
	input2 << "c_county3 = { development = 89 }\n";
	input2 << "c_county5 = { development = 99 }\n";
	const CK3::CountyDetails counties(input2);

	titles.linkCountyDetails(counties);
	const auto& c1 = titles.getFoundTitles().find("c_county3");
	const auto& c2 = titles.getFoundTitles().find("c_county5");

	ASSERT_EQ(89, c1->second->getCounty()->second->getDevelopment());
	ASSERT_EQ(99, c2->second->getCounty()->second->getDevelopment());
}

TEST(CK3World_LandedTitlesTests, titlesCanBeLinked)
{
	std::stringstream input;
	input << "d_duchy1 = { capital = c_county1\n c_county1 = { b_barony1 = { province = 22 } } } }\n";
	input << "d_duchy2 = { capital = c_county2\n c_county2 = { b_barony2 = { province = 25 } } }\n";
	CK3::LandedTitles clays;
	clays.loadTitles(input);

	std::stringstream input2;
	input2 << "13={key=\"c_county1\"\n coat_of_arms_id = 1}\n";
	input2 << "15={key=\"c_county2\"\n coat_of_arms_id = 2}\n";
	CK3::Titles titles(input2);

	clays.linkTitles(titles);
	const auto& c1 = clays.getFoundTitles().find("d_duchy1");
	const auto& c2 = clays.getFoundTitles().find("d_duchy2");

	ASSERT_EQ(1, c1->second->getCapital()->second->getCoA()->first);
	ASSERT_EQ(2, c2->second->getCapital()->second->getCoA()->first);
}

TEST(CK3World_LandedTitlesTests, missingTitlesLinkDoesNothing)
{
	std::stringstream input;
	input << "d_duchy1 = { capital = c_county1\n c_county1 = { b_barony1 = { province = 22 } } } }\n";
	input << "d_duchy2 = { capital = c_county2\n c_county2 = { b_barony2 = { province = 25 } } }\n";
	CK3::LandedTitles clays;
	clays.loadTitles(input);

	std::stringstream input2;
	input2 << "13={key=\"c_county1\"\n coat_of_arms_id = 1}\n";
	CK3::Titles titles(input2);

	clays.linkTitles(titles);
	const auto& c1 = clays.getFoundTitles().find("d_duchy1");
	const auto& c2 = clays.getFoundTitles().find("d_duchy2");

	ASSERT_EQ(1, c1->second->getCapital()->second->getCoA()->first);
	ASSERT_EQ(nullptr, c2->second->getCapital()->second);
}
