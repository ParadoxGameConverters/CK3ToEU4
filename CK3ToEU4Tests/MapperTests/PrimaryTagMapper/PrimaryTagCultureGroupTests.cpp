#include "../../CK3toEU4/Source/Mappers/PrimaryTagMapper/PrimaryTagCultureGroup.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_PrimaryTagCultureGroupTests, PrimaryTagsDefaultToEmpty)
{
	std::stringstream input;

	const mappers::PrimaryTagCultureGroup theMapping(input);

	ASSERT_TRUE(theMapping.getCollectedTags().empty());
}

TEST(Mappers_PrimaryTagCultureGroupTests, PrimaryTagsCanBeLoaded)
{
	std::stringstream input;
	input << "culture1 = { primary = TAG }\n";
	input << "culture2 = { }\n";
	input << "culture3 = { primary = GAT }\n";

	const mappers::PrimaryTagCultureGroup theMapping(input);

	const auto& theTags = theMapping.getCollectedTags();

	ASSERT_EQ(theTags.size(), 2);
	ASSERT_EQ("TAG", theTags.find("culture1")->second);
	ASSERT_EQ(theTags.end(), theTags.find("culture2"));
	ASSERT_EQ("GAT", theTags.find("culture3")->second);
}
