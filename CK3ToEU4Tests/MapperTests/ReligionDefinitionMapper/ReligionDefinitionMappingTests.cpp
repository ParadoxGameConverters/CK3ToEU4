#include "../../CK3toEU4/Source/Mappers/ReligionDefinitionMapper/ReligionDefinitionMapping.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_ReligionDefinitionMappingTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const mappers::ReligionDefinitionMapping theMapping(input);

	ASSERT_TRUE(theMapping.getCountry().empty());
	ASSERT_TRUE(theMapping.getCountrySecondary().empty());
	ASSERT_TRUE(theMapping.getProvince().empty());
	ASSERT_TRUE(theMapping.getUnique().empty());
	ASSERT_TRUE(theMapping.getNonUnique().empty());
}

TEST(Mappers_ReligionDefinitionMappingTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "country = { blah unformatted = = blah }\n";
	input << "country_as_secondary = { blah \nu\nnformatted = = blah }\n";
	input << "province = { blah unformatted = = blah }\n";
	input << "unique_modifiers = { blah unforma=t=ted = = blah }\n";
	input << "non_unique_modifiers = { blah unform\n\t\n\tatted = = blah }\n";
	const mappers::ReligionDefinitionMapping theMapping(input);

	// These tests demonstrate some of errors in singleItem, but these do not affect us in reality.

	ASSERT_EQ("blah unformatted ==blah ", theMapping.getCountry());				// singleItem strips whitespace post equal sign
	ASSERT_EQ("blah u\nnformatted ==blah ", theMapping.getCountrySecondary()); // singleItem strips newline post whitespace
	ASSERT_EQ("blah unformatted ==blah ", theMapping.getProvince());
	ASSERT_EQ("blah unforma=t=ted ==blah ", theMapping.getUnique());
	ASSERT_EQ("blah unform\natted ==blah ", theMapping.getNonUnique()); // singleItem strips whitespace post newline;
}
