#include "../../CK3toEU4/Source/Mappers/LocalizationMapper/LocalizationMapper.h"
#include "gtest/gtest.h"
#include "CommonFunctions.h"

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
	
	ASSERT_EQ("replaced value 1", locs.getLocBlockForKey("key1")->english);
	ASSERT_EQ("value \"subquoted\" 2", locs.getLocBlockForKey("key2")->english);
	ASSERT_EQ("valuee 1", locs.getLocBlockForKey("key1")->french);
	ASSERT_EQ("valuee \"subquoted\" 2", locs.getLocBlockForKey("key2")->french);
}

TEST(Mappers_LocalisationTests, localisationsReturnNulloptForMissingKey)
{
	const mappers::LocalizationMapper locs;

	ASSERT_FALSE(locs.getLocBlockForKey("key1"));
}

TEST(Mappers_LocalisationTests, localisationsReturnsEnglishForMissingLanguage)
{
	mappers::LocalizationMapper locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " key1: \"value 1\" # comment\n";
	locs.scrapeStream(input, "english");

	ASSERT_EQ("value 1", locs.getLocBlockForKey("key1")->french);
}
