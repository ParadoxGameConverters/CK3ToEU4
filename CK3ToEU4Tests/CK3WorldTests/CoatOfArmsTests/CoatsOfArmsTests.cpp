#include "../../CK3ToEU4/Source/CK3World/CoatsOfArms/CoatOfArms.h"
#include "../../CK3ToEU4/Source/CK3World/CoatsOfArms/CoatsOfArms.h"
#include "../../CK3ToEU4/Source/CK3World/Titles/Titles.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_CoatsOfArmsTests, CoatsOfArmsDefaultToEmpty)
{
	std::stringstream input;
	const CK3::CoatsOfArms coatsOfArms(input);

	ASSERT_TRUE(coatsOfArms.getCoats().empty());
}

TEST(CK3World_CoatsOfArmsTests, BundledCoatsOfArmsCanBeLoaded)
{
	std::stringstream input;
	input << "coat_of_arms_manager_database={\n";
	input << "\t13={parent=\"k_france\"}\n";
	input << "\t15={parent=\"k_england\"}\n";
	input << "}";

	const CK3::CoatsOfArms coatsOfArms(input);

	ASSERT_EQ(2, coatsOfArms.getCoats().size());
	ASSERT_EQ("k_france", coatsOfArms.getCoats().find(13)->second->getParent()->first);
	ASSERT_EQ("k_england", coatsOfArms.getCoats().find(15)->second->getParent()->first);
}

TEST(CK3World_CoatsOfArmsTests, UnBundledCoatsOfArmsCanBeLoaded)
{
	std::stringstream input;
	input << "13 = { parent= \"k_france\" }\n";
	input << "15 = { parent= \"k_england\" }\n";

	const CK3::CoatsOfArms coatsOfArms(input);

	ASSERT_EQ(2, coatsOfArms.getCoats().size());
	ASSERT_EQ("k_france", coatsOfArms.getCoats().find(13)->second->getParent()->first);
	ASSERT_EQ("k_england", coatsOfArms.getCoats().find(15)->second->getParent()->first);
}

TEST(CK3World_CoatsOfArmsTests, parentsCanBeLinked)
{
	std::stringstream input;
	input << "13={key=\"c_county\"\n coat_of_arms_id = 1}\n";
	input << "15={key=\"d_duchy\"\n coat_of_arms_id = 2}\n";
	const CK3::Titles titles(input);

	std::stringstream input2;
	input2 << "1 = { pattern=\"smooth\"}\n";
	input2 << "2 = { parent=\"c_county\"}\n";
	CK3::CoatsOfArms coats(input2);
	coats.linkParents(titles);

	const auto& c2 = coats.getCoats().find(2);

	ASSERT_EQ("smooth", c2->second->getParent()->second->getPattern());
}

TEST(CK3World_CoatsOfArmsTests, parentsLinkWithoutTitleThrowsException)
{
	std::stringstream input;
	input << "13={key=\"c_county\"\n coat_of_arms_id = 1}\n";
	input << "15={key=\"d_duchy\"\n coat_of_arms_id = 2}\n";
	const CK3::Titles titles(input);

	std::stringstream input2;
	input2 << "1 = { pattern=\"smooth\"}\n";
	input2 << "2 = { parent=\"c_county2\"}\n";
	CK3::CoatsOfArms coats(input2);

	ASSERT_THROW(coats.linkParents(titles), std::runtime_error);
}

TEST(CK3World_CoatsOfArmsTests, parentsLinkWithoutCoAIDThrowsException)
{
	std::stringstream input;
	input << "13={key=\"c_county\"\n}\n";
	input << "15={key=\"d_duchy\"\n coat_of_arms_id = 2}\n";
	const CK3::Titles titles(input);

	std::stringstream input2;
	input2 << "1 = { pattern=\"smooth\"}\n";
	input2 << "2 = { parent=\"c_county\"}\n";
	CK3::CoatsOfArms coats(input2);

	ASSERT_THROW(coats.linkParents(titles), std::runtime_error);
}

TEST(CK3World_CoatsOfArmsTests, parentsLinkWithInvalidCoAIDThrowsException)
{
	std::stringstream input;
	input << "13={key=\"c_county\"\n coat_of_arms_id = 3}\n";
	input << "15={key=\"d_duchy\"\n coat_of_arms_id = 2}\n";
	const CK3::Titles titles(input);

	std::stringstream input2;
	input2 << "1 = { pattern=\"smooth\"}\n";
	input2 << "2 = { parent=\"c_county2\"}\n";
	CK3::CoatsOfArms coats(input2);

	ASSERT_THROW(coats.linkParents(titles), std::runtime_error);
}
