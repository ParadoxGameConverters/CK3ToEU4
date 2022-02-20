#include "../../CK3toEU4/Source/Mappers/LocalizationMapper/LocalizationMapper.h"
#include "CommonFunctions.h"
#include "gtest/gtest.h"

TEST(Mappers_LocalisationTests, localisationsCanBeLoadedAndMatched)
{
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " key1: \"value 1\" # comment\n";
	input << " key2: \"value \"subquoted\" 2\"\n";
	std::stringstream input2;
	input2 << commonItems::utf8BOM << "l_french:\n";
	input2 << " key1: \"valuee 1\"\n";
	input2 << " key2: \"valuee \"subquoted\" 2\"\n";
	std::stringstream input3;
	input3 << commonItems::utf8BOM << "l_english:\n";
	input3 << " key1: \"replaced value 1\"\n";

	mappers::LocalizationMapper locs;
	locs.scrapeStream(input, "english");
	locs.scrapeStream(input2, "french");
	locs.scrapeStream(input3, "english");

	EXPECT_EQ("replaced value 1", locs.getLocBlockForKey("key1")->english);
	EXPECT_EQ("value \"subquoted\" 2", locs.getLocBlockForKey("key2")->english);
	EXPECT_EQ("valuee 1", locs.getLocBlockForKey("key1")->french);
	EXPECT_EQ("valuee \"subquoted\" 2", locs.getLocBlockForKey("key2")->french);
}

TEST(Mappers_LocalisationTests, localisationsReturnNulloptForMissingKey)
{
	const mappers::LocalizationMapper locs;

	EXPECT_FALSE(locs.getLocBlockForKey("key1"));
}

TEST(Mappers_LocalisationTests, localisationsReturnsEnglishForMissingLanguage)
{
	mappers::LocalizationMapper locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " key1: \"value 1\" # comment\n";
	locs.scrapeStream(input, "english");

	EXPECT_EQ("value 1", locs.getLocBlockForKey("key1")->french);
}

TEST(Mappers_LocalisationTests, reverseLookupCultureNameWorks)
{
	mappers::LocalizationMapper locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_korean:\n";
	input << " culture_name: \"The Name\"\n";
	locs.scrapeStream(input, "korean");

	EXPECT_EQ("culture_name", locs.reverseLookupCultureName("The Name"));
}

TEST(Mappers_LocalisationTests, reverseLookupCultureNameDoesNotWorkForNonCultures)
{
	mappers::LocalizationMapper locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_korean:\n";
	input << " d_culture_adj: \"The Name\"\n";
	locs.scrapeStream(input, "korean");

	EXPECT_EQ(std::nullopt, locs.reverseLookupCultureName("The Name"));
}
