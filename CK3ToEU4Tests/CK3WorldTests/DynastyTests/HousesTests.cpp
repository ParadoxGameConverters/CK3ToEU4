#include "../../CK3toEU4/Source/CK3World/Characters/Character.h"
#include "../../CK3toEU4/Source/CK3World/Characters/Characters.h"
#include "../../CK3toEU4/Source/CK3World/Dynasties/Dynasties.h"
#include "../../CK3toEU4/Source/CK3World/Dynasties/Dynasty.h"
#include "../../CK3toEU4/Source/CK3World/Dynasties/House.h"
#include "../../CK3toEU4/Source/CK3World/Dynasties/HouseNameScraper.h"
#include "../../CK3toEU4/Source/CK3World/Dynasties/Houses.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_HousesTests, HousesDefaultToEmpty)
{
	std::stringstream input;
	const CK3::Houses houses(input);

	EXPECT_TRUE(houses.getHouses().empty());
}

TEST(CK3World_HousesTests, HousesCanBeLoaded)
{
	std::stringstream input;
	input << "13={name=\"dynn_Villeneuve\"}\n";
	input << "15={name=\"dynn_Fournier\"}\n";

	const CK3::Houses houses(input);
	const auto& h1 = houses.getHouses().find(13);
	const auto& h2 = houses.getHouses().find(15);

	EXPECT_EQ("dynn_Villeneuve", h1->second->getName());
	EXPECT_EQ("dynn_Fournier", h2->second->getName());
}


TEST(CK3World_HousesTests, NonsenseHousesAreIgnored)
{
	std::stringstream input;
	input << "13=none\n";
	input << "15={name=\"dynn_Fournier\"}\n";

	const CK3::Houses houses(input);
	const auto& h1 = houses.getHouses().find(13);
	const auto& h2 = houses.getHouses().find(15);

	EXPECT_EQ(1, houses.getHouses().size());
	EXPECT_EQ(houses.getHouses().end(), h1);
	EXPECT_EQ("dynn_Fournier", h2->second->getName());
}

TEST(CK3World_HousesTests, charactersCanBeLinkedIgnoringMissing)
{
	std::stringstream input;
	input << "1={}\n";
	input << "2={first_name=\"bob\"}\n";
	input << "3={first_name=\"alice\"}\n";
	CK3::Characters characters;
	characters.loadCharacters(input);

	std::stringstream input2;
	input2 << "13={head_of_house = 2}\n";
	input2 << "15={head_of_house = 19}\n"; // missing character, often in the savegame. Deleted or something.
	auto houses = CK3::Houses(input2);
	houses.linkCharacters(characters);

	const auto& h1 = houses.getHouses().find(13);
	const auto& h2 = houses.getHouses().find(15);

	EXPECT_EQ("bob", h1->second->getHouseHead()->second->getName());
	EXPECT_FALSE(h2->second->getHouseHead());
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

	EXPECT_EQ(2, h1->second->getDynasty().second->getCoA()->first);
	EXPECT_EQ(1, h2->second->getDynasty().second->getCoA()->first);
}

TEST(CK3World_HousesTests, linkingMissingDynastyDoesNothing)
{
	std::stringstream input;
	input << "23={name=\"dynn_Villeneuve\"\n dynasty=15}\n";
	input << "25={name=\"dynn_Fournier\"\n dynasty=13}\n";
	CK3::Houses houses(input);

	std::stringstream input2;
	input2 << "15={coat_of_arms_id = 2}\n";
	const CK3::Dynasties dynasties(input2);
	houses.linkDynasties(dynasties);

	const auto& h1 = houses.getHouses().find(23);
	const auto& h2 = houses.getHouses().find(25);

	EXPECT_EQ(2, h1->second->getDynasty().second->getCoA()->first);
	EXPECT_EQ(nullptr, h2->second->getDynasty().second);
}

TEST(CK3World_HousesTests, namesCanBeImportedFromScraper)
{
	std::stringstream input;
	input << "23={key=\"house_vimaranes\"\n}\n";
	input << "25={key=\"house_cantabria\"\n}\n";
	CK3::Houses houses(input);

	std::stringstream input2;
	input2 << "house_vimaranes = { name = \"dynn_Vimaranes\" }\n";
	input2 << "house_cantabria = { prefix = \"dynnp_de\" name = \"dynn_Cantabria\" }\n";
	CK3::HouseNameScraper scraper;
	scraper.loadHouseDetails(input2);

	houses.importNames(scraper);

	const auto& h1 = houses.getHouses().find(23);
	const auto& h2 = houses.getHouses().find(25);

	EXPECT_EQ("dynn_Vimaranes", h1->second->getName());
	EXPECT_EQ("dynnp_de", h2->second->getPrefix());
	EXPECT_EQ("dynn_Cantabria", h2->second->getName());
}