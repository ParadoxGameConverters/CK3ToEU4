#include "../CK3toEU4/Source/Mappers/TraitScraper/TraitScraper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_TraitScraperTests, traitsDefaultToBlank)
{
	std::stringstream input;

	mappers::TraitScraper traitScraper;
	traitScraper.loadTraits(input);

	ASSERT_TRUE(traitScraper.getTraits().empty());
}

TEST(Mappers_TraitScraperTests, traitsWithoutIndexAreIgnored)
{
	std::stringstream input;
	input << "trait1 = {}\n";
	input << "trait2 = {}\n";
	input << "trait3 = {}\n";

	mappers::TraitScraper traitScraper;
	traitScraper.loadTraits(input);

	ASSERT_TRUE(traitScraper.getTraits().empty());
}

TEST(Mappers_TraitScraperTests, traitsWithIndexZeroAreIgnored)
{
	std::stringstream input;
	input << "trait1 = { index = 0 }\n";

	mappers::TraitScraper traitScraper;
	traitScraper.loadTraits(input);

	ASSERT_TRUE(traitScraper.getTraits().empty());
}

TEST(Mappers_TraitScraperTests, traitsWithIndexAreLoaded)
{
	std::stringstream input;
	input << "trait1 = { index = 4 }\n";
	input << "trait2 = { index = 6 }\n";
	input << "trait3 = { index = 8 }\n";

	mappers::TraitScraper traitScraper;
	traitScraper.loadTraits(input);

	ASSERT_EQ(3, traitScraper.getTraits().size());
	ASSERT_EQ("trait1", traitScraper.getTraits().find(4)->second);
	ASSERT_EQ("trait2", traitScraper.getTraits().find(6)->second);
	ASSERT_EQ("trait3", traitScraper.getTraits().find(8)->second);
}

TEST(Mappers_TraitScraperTests, traitsCanBeOverriddenByMods)
{
	std::stringstream input;
	input << "trait1 = { index = 4 }\n";
	input << "trait2 = { index = 6 }\n";
	input << "trait3 = { index = 8 }\n";

	mappers::TraitScraper traitScraper;
	traitScraper.loadTraits(input);

	std::stringstream input2;
	input2 << "nutrait1 = { index = 4 }\n";
	input2 << "nutrait2 = { index = 6 }\n";
	input2 << "nutrait3 = { index = 8 }\n";
	traitScraper.loadTraits(input2);

	ASSERT_EQ(3, traitScraper.getTraits().size());
	ASSERT_EQ("nutrait1", traitScraper.getTraits().find(4)->second);
	ASSERT_EQ("nutrait2", traitScraper.getTraits().find(6)->second);
	ASSERT_EQ("nutrait3", traitScraper.getTraits().find(8)->second);
}

TEST(Mappers_TraitScraperTests, traitsCanMatchForID)
{
	std::stringstream input;
	input << "trait1 = { index = 4 }\n";
	input << "trait2 = { index = 6 }\n";
	input << "trait3 = { index = 8 }\n";

	mappers::TraitScraper traitScraper;
	traitScraper.loadTraits(input);

	ASSERT_EQ("trait2", traitScraper.getTraitForID(6));
}

TEST(Mappers_TraitScraperTests, traitsReturnNullforMismatch)
{
	std::stringstream input;
	input << "trait1 = { index = 4 }\n";
	input << "trait2 = { index = 6 }\n";
	input << "trait3 = { index = 8 }\n";

	mappers::TraitScraper traitScraper;
	traitScraper.loadTraits(input);

	ASSERT_FALSE(traitScraper.getTraitForID(5));
}
