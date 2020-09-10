#include "../../CK3toEU4/Source/CK3World/Religions/Religion.h"
#include "../../CK3toEU4/Source/CK3World/Religions/Religions.h"
#include "../../CK3toEU4/Source/CK3World/Religions/Faith.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_ReligionsTests, ReligionsDefaultToEmpty)
{
	std::stringstream input;
	const CK3::Religions religions(input);

	ASSERT_TRUE(religions.getReligions().empty());
}

TEST(CK3World_ReligionsTests, BundledReligionsCanBeLoaded)
{
	std::stringstream input;
	input << "religions={\n";
	input << "\t13={tag=\"bon_religion\"}\n";
	input << "\t15={tag=\"buddhism_religion\"}\n";
	input << "}";

	const CK3::Religions religions(input);
	const auto& d1 = religions.getReligions().find(13);
	const auto& d2 = religions.getReligions().find(15);

	ASSERT_EQ(2, religions.getReligions().size());
	ASSERT_EQ("bon_religion", d1->second->getName());
	ASSERT_EQ("buddhism_religion", d2->second->getName());
}

TEST(CK3World_ReligionsTests, UnBundledReligionsCanBeLoaded)
{
	std::stringstream input;
	input << "13={tag=\"bon_religion\"}\n";
	input << "15={tag=\"buddhism_religion\"}\n";

	const CK3::Religions religions(input);
	const auto& d1 = religions.getReligions().find(13);
	const auto& d2 = religions.getReligions().find(15);

	ASSERT_EQ(2, religions.getReligions().size());
	ASSERT_EQ("bon_religion", d1->second->getName());
	ASSERT_EQ("buddhism_religion", d2->second->getName());
}

TEST(CK3World_ReligionsTests, faithsCanBeLinked)
{
	std::stringstream input;
	input << "13={tag=\"old_bon\"}\n";
	input << "15={tag=\"theravada\"}\n";
	const CK3::Faiths faiths(input);

	std::stringstream input2;
	input2 << "1 = { tag=\"bon_religion\"\n faiths = { 15 } }\n";
	input2 << "2 = { tag=\"buddhism_religion\"\n faiths = { 13 15 } }\n";
	CK3::Religions religions(input2);
	religions.linkFaiths(faiths);

	const auto& r1 = religions.getReligions().find(1);
	const auto& r2 = religions.getReligions().find(2);

	const auto& r1Faiths = r1->second->getFaiths();
	const auto& r2Faiths = r2->second->getFaiths();
	
	ASSERT_EQ(1, r1Faiths.size());
	ASSERT_EQ("theravada", r1Faiths.find(15)->second->getName());
	ASSERT_EQ(2, r2Faiths.size());
	ASSERT_EQ("theravada", r2Faiths.find(15)->second->getName());
	ASSERT_EQ("old_bon", r2Faiths.find(13)->second->getName());
}

TEST(CK3World_ReligionsTests, linkingMissingFaithThrowsException)
{
	std::stringstream input;
	input << "13={tag=\"old_bon\"}\n";
	const CK3::Faiths faiths(input);

	std::stringstream input2;
	input2 << "1 = { tag=\"bon_religion\"\n faiths = { 13 } }\n";
	input2 << "2 = { tag=\"buddhism_religion\"\n faiths = { 13 15 } }\n";
	CK3::Religions religions(input2);

	ASSERT_THROW(religions.linkFaiths(faiths), std::runtime_error);
}
