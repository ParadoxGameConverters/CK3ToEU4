#include "../../CK3ToEU4/Source/Mappers/ReligionGroupScraper/ReligionGroupScraping.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_ReligionGroupScrapingTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const mappers::ReligionGroupScraping theMapping(input);

	ASSERT_TRUE(theMapping.getReligionNames().empty());
}

TEST(Mappers_ReligionGroupScrapingTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "religion1 = { blah blah }\n";
	input << "some_religion = { blah unformatted blah }\n";
	input << "religion1 = { blah unformatted raw text blah }\n";
	input << "some_other_religion = { blah unformatted blah }\n";
	const mappers::ReligionGroupScraping theMapping(input);

	ASSERT_EQ(3, theMapping.getReligionNames().size());
	ASSERT_TRUE(theMapping.getReligionNames().count("religion1"));
	ASSERT_TRUE(theMapping.getReligionNames().count("some_religion"));
	ASSERT_TRUE(theMapping.getReligionNames().count("some_other_religion"));
}
