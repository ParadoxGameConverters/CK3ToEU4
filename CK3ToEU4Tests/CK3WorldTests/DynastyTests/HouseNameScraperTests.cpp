#include "../CK3ToEU4/Source/CK3World/Dynasties/HouseNameScraper.h"
#include "gtest/gtest.h"

TEST(CK3World_HouseNameScraperTests, startingMapIsEmpty)
{
	const CK3::HouseNameScraper scraper;

	EXPECT_TRUE(scraper.getHouseNames().empty());
}

TEST(CK3World_HouseTests, housesCanBeScraped)
{
	std::stringstream input;
	input << "house_cantabria = { prefix = \"dynnp_de\" name = \"dynn_Cantabria\" dynasty = erwigiana }\n";
	input << "house_betotez = { name = \"dynn_BetO_tez\" dynasty = erwigiana }\n";

	CK3::HouseNameScraper scraper;
	scraper.loadHouseDetails(input);

	EXPECT_EQ(2, scraper.getHouseNames().size());
	EXPECT_EQ("dynnp_de", scraper.getHouseNames().at("house_cantabria").getPrefix());
	EXPECT_EQ("dynn_Cantabria", scraper.getHouseNames().at("house_cantabria").getName());
	EXPECT_EQ("dynn_BetO_tez", scraper.getHouseNames().at("house_betotez").getName());
}

TEST(CK3World_HouseTests, houseNamesCanBeRequested)
{
	std::stringstream input;
	input << "house_cantabria = { prefix = \"dynnp_de\" name = \"dynn_Cantabria\" dynasty = erwigiana }\n";
	input << "house_betotez = { name = \"dynn_BetO_tez\" dynasty = erwigiana }\n";

	CK3::HouseNameScraper scraper;
	scraper.loadHouseDetails(input);

	EXPECT_EQ("dynnp_de", scraper.getPrefixForKey("house_cantabria"));
	EXPECT_EQ("dynn_Cantabria", scraper.getNameForKey("house_cantabria"));
	EXPECT_EQ("dynn_BetO_tez", scraper.getNameForKey("house_betotez"));
}

TEST(CK3World_HouseTests, houseNamesRequestMismatchesReturnNull)
{
	std::stringstream input;
	CK3::HouseNameScraper scraper;
	scraper.loadHouseDetails(input);

	EXPECT_EQ(std::nullopt, scraper.getPrefixForKey("house_cantabria"));
	EXPECT_EQ(std::nullopt, scraper.getNameForKey("house_cantabria"));
}
