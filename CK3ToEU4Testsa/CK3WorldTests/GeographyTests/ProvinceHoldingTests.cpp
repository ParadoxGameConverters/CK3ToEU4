#include "../CK3ToEU4/Source/CK3World/Geography/ProvinceHolding.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_ProvinceHoldingTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::ProvinceHolding holding(input);

	ASSERT_TRUE(holding.getHoldingType().empty());
	ASSERT_TRUE(holding.getSpecialBuilding().empty());
	ASSERT_TRUE(holding.getBuildings().empty());
}

TEST(CK3World_ProvinceHoldingTests, blankHoldingIsBlank)
{
	std::stringstream input;
	input << "holding = {}";

	const CK3::ProvinceHolding holding(input);

	ASSERT_TRUE(holding.getHoldingType().empty());
	ASSERT_TRUE(holding.getSpecialBuilding().empty());
	ASSERT_TRUE(holding.getBuildings().empty());
}

TEST(CK3World_ProvinceHoldingTests, holdingPrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "holding = {\n";
	input << "\ttype = \"city_holding\"\n";
	input << "\tspecial_building_type = \"holy_site_cathedral_01\"\n";
	input << "}";

	const CK3::ProvinceHolding holding(input);

	ASSERT_EQ("city_holding", holding.getHoldingType());
	ASSERT_EQ("holy_site_cathedral_01", holding.getSpecialBuilding());
}

TEST(CK3World_ProvinceHoldingTests, holdingEmptyBuildingBlobsAreIgnored)
{
	std::stringstream input;
	input << "holding = {\n";
	input << "\tbuildings = {\n";
	input << "\t\n{\n}\n{\n}\n{\n}{}{}\n";
	input << "\t}\n";
	input << "}";

	const CK3::ProvinceHolding holding(input);

	ASSERT_TRUE(holding.getBuildings().empty());
}

TEST(CK3World_ProvinceHoldingTests, holdingBuildingBlobsCanBeLoaded)
{
	std::stringstream input;
	input << "holding = {\n";
	input << "\tbuildings = {\n";
	input << "\t\n{\ntype = \"castle_01\"\n}\n{}\n{\n}{}{type = \"hill_farms_02\"}\n";
	input << "\t}\n";
	input << "}";

	const CK3::ProvinceHolding holding(input);

	ASSERT_EQ(2, holding.getBuildings().size());
	ASSERT_EQ(1, holding.getBuildings().count("castle_01"));
	ASSERT_EQ(1, holding.getBuildings().count("hill_farms_02"));
}

TEST(CK3World_ProvinceHoldingTests, holdingBuildingsCanBeCounted)
{
	std::stringstream input;
	input << "holding = {\n";
	input << "\tbuildings = {\n";
	input << "\t\n{\ntype = \"castle_01\"\n}\n{}\n{\n}{}{type = \"hill_farms_02\"}\n"; // 2
	input << "\t}\n";
	input << "}";

	const CK3::ProvinceHolding holding(input);

	// 1 castle + 2 farms

	ASSERT_EQ(3, holding.countBuildings());
}

TEST(CK3World_ProvinceHoldingTests, holdingAllBuildingsCanBeCounted)
{
	std::stringstream input;
	input << "holding = {\n";
	input << "\tspecial_building_type = \"holy_site_cathedral_01\"\n"; // 1
	input << "\tbuildings = {\n";
	input << "\t\n{\ntype = \"castle_01\"\n}\n{}\n{\n}{}{type = \"hill_farms_02\"}\n"; // +2
	input << "\t}\n";
	input << "}";

	const CK3::ProvinceHolding holding(input);

	ASSERT_EQ(4, holding.countBuildings());
}
