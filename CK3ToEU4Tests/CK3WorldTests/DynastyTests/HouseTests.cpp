#include "../CK3toEU4/Source/CK3World/Dynasties/House.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_HouseTests, houseIDLoads)
{
	std::stringstream input;
	const CK3::House house(input, 42);

	EXPECT_EQ(42, house.getID());
}

TEST(CK3World_HouseTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::House house(input, 1);

	EXPECT_TRUE(house.getKey().empty());
	EXPECT_TRUE(house.getName().empty());
	EXPECT_TRUE(house.getLocalizedName().empty());
	EXPECT_TRUE(house.getPrefix().empty());
	EXPECT_TRUE(house.getDynasty().first);
}

TEST(CK3World_HouseTests, housePrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "key = \"house_vimaranes\"\n";
	input << "name = \"dynn_Villeneuve\"\n";
	input << "localized_name = \"Gaye\"\n";
	input << "prefix = \"dynnp_de\"\n";
	input << "dynasty = 19\n";

	const CK3::House house(input, 42);

	EXPECT_EQ("house_vimaranes", house.getKey());
	EXPECT_EQ("dynn_Villeneuve", house.getName());
	EXPECT_EQ("Gaye", house.getLocalizedName());
	EXPECT_EQ("dynnp_de", house.getPrefix());
	EXPECT_EQ(19, house.getDynasty().first);
}
