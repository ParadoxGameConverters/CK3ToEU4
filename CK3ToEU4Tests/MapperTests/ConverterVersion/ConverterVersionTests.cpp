#include "../../CK3toEU4/Source/Mappers/ConverterVersion/ConverterVersion.h"
#include "gtest/gtest.h"

TEST(Mappers_ConverterVersionTests, PrimitivesDefaultToEmpty)
{
	std::stringstream input;
	const mappers::ConverterVersion version(input);

	EXPECT_TRUE(version.getName().empty());
	EXPECT_TRUE(version.getVersion().empty());
}

TEST(Mappers_ConverterVersionTests, NameCanBeSet)
{
	std::stringstream input;
	input << "name = \"Test Name\"";
	const mappers::ConverterVersion version(input);

	EXPECT_EQ("Test Name", version.getName());
}

TEST(Mappers_ConverterVersionTests, VersionCanBeSet)
{
	std::stringstream input;
	input << "version = \"5.4-Test\"";
	const mappers::ConverterVersion version(input);

	EXPECT_EQ("5.4-Test", version.getVersion());
}

TEST(Mappers_ConverterVersionTests, DescriptionCanBeConstructed)
{
	std::stringstream input;
	input << "source = \"CK3\"\n";
	input << "target = \"EU4\"\n";
	input << "minSource = \"1.3\"\n";
	input << "maxSource = \"1.4\"\n";
	input << "minTarget = \"1.30.6\"\n";
	input << "maxTarget = \"1.31.4\"\n";
	const mappers::ConverterVersion version(input);

	EXPECT_EQ("Compatible with CK3 [v1.3-v1.4] and EU4 [v1.30.6-v1.31.4]", version.getDescription());
}

TEST(Mappers_ConverterVersionTests, DescriptionDoesNotDuplicateVersions)
{
	std::stringstream input;
	input << "source = \"CK3\"\n";
	input << "target = \"EU4\"\n";
	input << "minSource = \"1.3\"\n";
	input << "maxSource = \"1.3\"\n";
	input << "minTarget = \"1.31\"\n";
	input << "maxTarget = \"1.31\"\n";
	const mappers::ConverterVersion version(input);

	EXPECT_EQ("Compatible with CK3 [v1.3] and EU4 [v1.31]", version.getDescription());
}