#include "../../CK3toEU4/Source/Mappers/ConverterVersion/ConverterVersion.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_ConverterVersionTests, AllItemsDefaultToEmpty)
{
	std::stringstream input;
	mappers::ConverterVersion version(input);

	EXPECT_TRUE(version.getName().empty());
	EXPECT_TRUE(version.getDescription().empty());
	EXPECT_TRUE(version.getVersion().empty());
}

TEST(Mappers_ConverterVersionTests, NameCanBeSet)
{
	std::stringstream input;
	input << "name = \"Test Name\"";
	mappers::ConverterVersion version(input);

	EXPECT_EQ("Test Name", version.getName());
}

TEST(Mappers_ConverterVersionTests, VersionCanBeSet)
{
	std::stringstream input;
	input << "version = \"5.4-Test\"";
	mappers::ConverterVersion version(input);

	std::stringstream output;
	output << version;

	EXPECT_EQ("5.4-Test", version.getVersion());
}

TEST(Mappers_ConverterVersionTests, DescriptionCanBeSet)
{
	std::stringstream input;
	input << "descriptionLine = \"test description\"";
	mappers::ConverterVersion version(input);

	EXPECT_EQ("test description", version.getDescription());
}