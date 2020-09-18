#include "../../CK3toEU4/Source/CK3World/Dynasties/House.h"
#include "../../CK3toEU4/Source/CK3World/Dynasties/Houses.h"
#include "../../CK3toEU4/Source/CK3World/Dynasties/Dynasty.h"
#include "../../CK3toEU4/Source/CK3World/Dynasties/Dynasties.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_HousesTests, HousesDefaultToEmpty)
{
	std::stringstream input;
	const CK3::Houses houses(input);

	ASSERT_TRUE(houses.getHouses().empty());
}

TEST(CK3World_HousesTests, HousesCanBeLoaded)
{
	std::stringstream input;
	input << "13={name=\"dynn_Villeneuve\"}\n";
	input << "15={name=\"dynn_Fournier\"}\n";

	const CK3::Houses houses(input);
	const auto& h1 = houses.getHouses().find(13);
	const auto& h2 = houses.getHouses().find(15);

	ASSERT_EQ("dynn_Villeneuve", h1->second->getName());
	ASSERT_EQ("dynn_Fournier", h2->second->getName());
}


TEST(CK3World_HousesTests, NonsenseHousesAreIgnored)
{
	std::stringstream input;
	input << "13=none\n";
	input << "15={name=\"dynn_Fournier\"}\n";

	const CK3::Houses houses(input);
	const auto& h1 = houses.getHouses().find(13);
	const auto& h2 = houses.getHouses().find(15);

	ASSERT_EQ(1, houses.getHouses().size());
	ASSERT_EQ(houses.getHouses().end(), h1);
	ASSERT_EQ("dynn_Fournier", h2->second->getName());
}

TEST(CK3World_HousesTests, dynastiesCanBeLinked)
{
	std::stringstream input;
	input << "23={name=\"dynn_Villeneuve\"\n dynasty=15}\n";
	input << "25={name=\"dynn_Fournier\"\n dynasty=13}\n";
	CK3::Houses houses(input);
	
	std::stringstream input2;
	input2 << "13={coat_of_arms_id = 1}\n";
	input2 << "15={coat_of_arms_id = 2}\n";	
	const CK3::Dynasties dynasties(input2);
	houses.linkDynasties(dynasties);

	const auto& h1 = houses.getHouses().find(23);
	const auto& h2 = houses.getHouses().find(25);

	ASSERT_EQ(2, h1->second->getDynasty().second->getCoA()->first);
	ASSERT_EQ(1, h2->second->getDynasty().second->getCoA()->first);
}

TEST(CK3World_HousesTests, linkingMissingDynastyThrowsException)
{
	std::stringstream input;
	input << "23={name=\"dynn_Villeneuve\"\n dynasty=15}\n";
	input << "25={name=\"dynn_Fournier\"\n dynasty=13}\n";
	CK3::Houses houses(input);

	std::stringstream input2;
	input2 << "15={coat_of_arms_id = 2}\n";
	const CK3::Dynasties dynasties(input2);

	ASSERT_THROW(houses.linkDynasties(dynasties), std::runtime_error);
}
