#include "../../CK3toEU4/Source/Mappers/PrimaryTagMapper/PrimaryTagMapper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_PrimaryTagMapperTests, CultureTagsDefaultToEmpty)
{
	std::stringstream input;

	const mappers::PrimaryTagMapper tagMapper(input);

	ASSERT_TRUE(tagMapper.getCultureTags().empty());
}

TEST(Mappers_PrimaryTagMapperTests, CultureTagsCanBeLoaded)
{
	std::stringstream input;
	input << "group1 = { culture1 = {primary = TAG} culture2 = {} culture3 = {primary = GAT} }\n";
	input << "group2 = { culture4 = {primary = GAT} culture5 = {} culture6 = {primary = GOT} }\n";
	const mappers::PrimaryTagMapper tagMapper(input);

	ASSERT_EQ(tagMapper.getCultureTags().size(), 4);
}

TEST(Mappers_PrimaryTagMapperTests, TagMapperReturnsNullOnMiss)
{
	std::stringstream input;
	input << "group1 = { culture1 = {primary = TAG} culture2 = {} culture3 = {primary = GAT} }\n";
	input << "group2 = { culture4 = {primary = GAT} culture5 = {} culture6 = {primary = GOT} }\n";
	const mappers::PrimaryTagMapper tagMapper(input);

	ASSERT_FALSE(tagMapper.getPrimaryTagForCulture("culture2"));
	ASSERT_FALSE(tagMapper.getPrimaryTagForCulture("culture5"));
}

TEST(Mappers_PrimaryTagMapperTests, TagMapperReturnsTagOnHit)
{
	std::stringstream input;
	input << "group1 = { culture1 = {primary = TAG} culture2 = {} culture3 = {primary = GAT} }\n";
	input << "group2 = { culture4 = {primary = GAT} culture5 = {} culture6 = {primary = GOT} }\n";
	const mappers::PrimaryTagMapper tagMapper(input);

	ASSERT_EQ("TAG", *tagMapper.getPrimaryTagForCulture("culture1"));
	ASSERT_EQ("GAT", *tagMapper.getPrimaryTagForCulture("culture3"));
	ASSERT_EQ("GAT", *tagMapper.getPrimaryTagForCulture("culture4"));
	ASSERT_EQ("GOT", *tagMapper.getPrimaryTagForCulture("culture6"));
}
