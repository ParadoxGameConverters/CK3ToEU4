#include "../../CK3toEU4/Source/ConverterVersion/ConverterVersion.h"
#include "../../CK3toEU4/Source/ConverterVersion/outConverterVersion.h"
#include "gtest/gtest.h"
#include <sstream>



TEST(ConverterVersionTests, AllItemsDefaultToEmpty)
{
	std::stringstream input;
	ConverterVersion::ConverterVersion version(input);

	std::stringstream output;
	output << version;

	EXPECT_EQ("Converter version  \"\" - ", output.str());
}


TEST(ConverterVersionTests, NameCanBeSet)
{
	std::stringstream input;
	input << "name = \"Test Name\"";
	ConverterVersion::ConverterVersion version(input);

	std::stringstream output;
	output << version;

	EXPECT_EQ("Converter version  \"Test Name\" - ", output.str());
}


TEST(ConverterVersionTests, VersionCanBeSet)
{
	std::stringstream input;
	input << "version = \"5.4-Test\"";
	ConverterVersion::ConverterVersion version(input);

	std::stringstream output;
	output << version;

	EXPECT_EQ("Converter version 5.4-Test \"\" - ", output.str());
}


TEST(ConverterVersionTests, DescriptionCanBeSet)
{
	std::stringstream input;
	input << "descriptionLine = \"test description\"";
	ConverterVersion::ConverterVersion version(input);

	std::stringstream output;
	output << version;

	EXPECT_EQ("Converter version  \"\" - test description", output.str());
}