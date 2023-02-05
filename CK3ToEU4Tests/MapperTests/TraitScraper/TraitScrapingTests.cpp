#include "../CK3ToEU4/Source/Mappers/TraitScraper/TraitScraping.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_TraitScrapingTests, primitivesDefaultToBlank)
{
	std::stringstream input;

	const auto traitScraping = mappers::TraitScraping(input);

	ASSERT_FALSE(traitScraping.getIndex());
}

TEST(Mappers_TraitScrapingTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "stuff = irrelevant\n";
	input << "index = 14\n";
	input << "irrelevant = stuff\n";

	const auto traitScraping = mappers::TraitScraping(input);

	ASSERT_EQ(14, traitScraping.getIndex());
}
