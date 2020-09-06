#include "../CK3toEU4/Source/CK3World/Geography/BaronyHolding.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_BaronyHoldingTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::BaronyHolding holding(input);

	ASSERT_TRUE(holding.getHoldingType().empty());
	ASSERT_TRUE(holding.getSpecialBuilding().empty());
	ASSERT_TRUE(holding.getBuildings().empty());
}

TEST(CK3World_BaronyHoldingTests, blankHoldingIsBlank)
{
	std::stringstream input;
	input << "holding = {}";

	const CK3::BaronyHolding holding(input);

	ASSERT_TRUE(holding.getHoldingType().empty());
	ASSERT_TRUE(holding.getSpecialBuilding().empty());
	ASSERT_TRUE(holding.getBuildings().empty());
}

TEST(CK3World_BaronyHoldingTests, holdingPrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "holding = {\n";
	input << "\ttype = \"city_holding\"\n";
	input << "\tspecial_building_type = \"holy_site_cathedral_01\"\n";
	input << "}";

	const CK3::BaronyHolding holding(input);

	ASSERT_EQ("city_holding", holding.getHoldingType());
	ASSERT_EQ("holy_site_cathedral_01", holding.getSpecialBuilding());
}

TEST(CK3World_BaronyHoldingTests, holdingEmptyBuildingBlobsAreIgnored)
{
	std::stringstream input;
	input << "holding = {\n";
	input << "\tbuildings = {\n";
	input << "\t\n{\n}\n{\n}\n{\n}{}{}\n";
	input << "\t}\n";
	input << "}";

	const CK3::BaronyHolding holding(input);

	ASSERT_TRUE(holding.getBuildings().empty());
}

TEST(CK3World_BaronyHoldingTests, holdingBuildingBlobsCanBeLoaded)
{
	std::stringstream input;
	input << "holding = {\n";
	input << "\tbuildings = {\n";
	input << "\t\n{\ntype = \"castle_01\"\n}\n{}\n{\n}{}{type = \"hill_farms_02\"}\n";
	input << "\t}\n";
	input << "}";

	const CK3::BaronyHolding holding(input);

	ASSERT_EQ(2, holding.getBuildings().size());
	ASSERT_EQ(1, holding.getBuildings().count("castle_01"));
	ASSERT_EQ(1, holding.getBuildings().count("hill_farms_02"));
}
