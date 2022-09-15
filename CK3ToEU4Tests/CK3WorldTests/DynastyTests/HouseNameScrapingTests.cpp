#include "../CK3toEU4/Source/CK3World/Dynasties/HouseNameScraping.h"
#include "gtest/gtest.h"

TEST(CK3World_HouseNameScrapingTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::HouseNameScraping scraping(input);

	EXPECT_TRUE(scraping.getName().empty());
	EXPECT_TRUE(scraping.getPrefix().empty());
}

TEST(CK3World_HouseNameScrapingTests, houseNamesCanBeLoaded)
{
	std::stringstream input;
	input << "name = \"dynn_Villeneuve\"\n";
	input << "prefix = \"dynnp_de\"\n";
	input << "dynasty = somedynasty\n"; // ignored

	const CK3::HouseNameScraping scraping(input);

	EXPECT_EQ("dynn_Villeneuve", scraping.getName());
	EXPECT_EQ("dynnp_de", scraping.getPrefix());
}
