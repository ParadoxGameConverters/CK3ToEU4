#include "../CK3toEU4/Source/CK3World/Dynasties/House.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_HouseTests, houseIDLoads)
{
	std::stringstream input;
	const CK3::House house(input, 42);

	ASSERT_EQ(42, house.getID());
}

TEST(CK3World_HouseTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::House house(input, 1);

	ASSERT_TRUE(house.getKey().empty());
	ASSERT_TRUE(house.getName().empty());
	ASSERT_TRUE(house.getLocalizedName().empty());
	ASSERT_TRUE(house.getPrefix().empty());
	ASSERT_FALSE(house.getDynasty().first);
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
	ASSERT_EQ("dynn_Villeneuve", house.getName());
	ASSERT_EQ("Gaye", house.getLocalizedName());
	ASSERT_EQ("dynnp_de", house.getPrefix());
	ASSERT_EQ(19, house.getDynasty().first);
}
