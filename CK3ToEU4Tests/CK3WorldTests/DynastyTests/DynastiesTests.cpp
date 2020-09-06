#include "../../CK3toEU4/Source/CK3World/Dynasties/Dynasty.h"
#include "../../CK3toEU4/Source/CK3World/Dynasties/Dynasties.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_DynastiesTests, DynastiesDefaultToEmpty)
{
	std::stringstream input;
	const CK3::Dynasties dynasties(input);

	ASSERT_TRUE(dynasties.getDynasties().empty());
}

TEST(CK3World_DynastiesTests, BundledDynastiesCanBeLoaded)
{
	std::stringstream input;
	input << "dynasties={\n";
	input << "\t13={key=\"2\"}\n";
	input << "\t15={key=\"7\"}\n";
	input << "}";

	const CK3::Dynasties dynasties(input);
	const auto& d1 = dynasties.getDynasties().find(13);
	const auto& d2 = dynasties.getDynasties().find(15);

	ASSERT_EQ(2, dynasties.getDynasties().size());
	ASSERT_EQ("2", d1->second->getDynID());
	ASSERT_EQ("7", d2->second->getDynID());
}

TEST(CK3World_DynastiesTests, UnBundledDynastiesCanBeLoaded)
{
	std::stringstream input;
	input << "13={key=\"2\"}\n";
	input << "15={key=\"7\"}\n";

	const CK3::Dynasties dynasties(input);
	const auto& d1 = dynasties.getDynasties().find(13);
	const auto& d2 = dynasties.getDynasties().find(15);

	ASSERT_EQ(2, dynasties.getDynasties().size());
	ASSERT_EQ("2", d1->second->getDynID());
	ASSERT_EQ("7", d2->second->getDynID());
}
