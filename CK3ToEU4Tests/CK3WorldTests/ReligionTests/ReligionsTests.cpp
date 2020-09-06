#include "../../CK3toEU4/Source/CK3World/Religions/Religion.h"
#include "../../CK3toEU4/Source/CK3World/Religions/Religions.h"
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
