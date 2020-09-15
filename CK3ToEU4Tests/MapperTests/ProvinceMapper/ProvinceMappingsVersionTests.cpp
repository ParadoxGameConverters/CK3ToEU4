#include "../CK3toEU4/Source/Mappers/ProvinceMapper/ProvinceMappingsVersion.h"
#include "gtest/gtest.h"
#include <sstream>
#include <string>

TEST(Mappers_ProvinceMappingsVersionTests, MappingsDefaultToEmpty)
{
	std::stringstream input;
	input << "= {\n";
	input << "}";

	mappers::ProvinceMappingsVersion theMappingVersion(input);

	ASSERT_TRUE(theMappingVersion.getMappings().empty());
}

TEST(Mappers_ProvinceMappingsVersionTests, MappingsCanBeLoaded)
{
	std::stringstream input;
	input << "= {\n";
	input << "	link = { eu4 = 1 ck3 = 1 }\n";
	input << "	link = { eu4 = 2 ck3 = 2 }\n";
	input << "}";

	mappers::ProvinceMappingsVersion theMappingVersion(input);

	ASSERT_EQ(2, theMappingVersion.getMappings().size());
}