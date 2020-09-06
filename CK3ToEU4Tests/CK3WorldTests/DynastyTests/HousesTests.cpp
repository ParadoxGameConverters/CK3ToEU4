#include "../../CK3toEU4/Source/CK3World/Dynasties/House.h"
#include "../../CK3toEU4/Source/CK3World/Dynasties/Houses.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_HousesTests, HousesDefaultToEmpty)
{
	std::stringstream input;
	const CK3::Houses characters(input);

	ASSERT_TRUE(characters.getHouses().empty());
}

TEST(CK3World_HousesTests, HousesCanBeLoaded)
{
	std::stringstream input;
	input << "13={name=\"dynn_Villeneuve\"}\n";
	input << "15={name=\"dynn_Fournier\"}\n";

	const CK3::Houses characters(input);
	const auto& h1 = characters.getHouses().find(13);
	const auto& h2 = characters.getHouses().find(15);

	ASSERT_EQ("dynn_Villeneuve", h1->second->getName());
	ASSERT_EQ("dynn_Fournier", h2->second->getName());
}
