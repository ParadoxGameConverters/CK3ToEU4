#include "../CK3toEU4/Source/CK3World/Dynasties/Dynasty.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_DynastyTests, dynastyIDLoads)
{
	std::stringstream input;
	const CK3::Dynasty dynasty(input, 42);

	ASSERT_EQ(42, dynasty.getGameID());
}

TEST(CK3World_DynastyTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::Dynasty dynasty(input, 1);

	ASSERT_TRUE(dynasty.getDynID().empty());
	ASSERT_FALSE(dynasty.isAppropriateRealmName());
	ASSERT_FALSE(dynasty.getCoA().first);
}

TEST(CK3World_DynastyTests, dynastyPrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "key = \"7\"\n";
	input << "good_for_realm_name = yes\n";
	input << "coat_of_arms_id = 14\n";

	const CK3::Dynasty dynasty(input, 42);

	ASSERT_EQ("7", dynasty.getDynID());
	ASSERT_TRUE(dynasty.isAppropriateRealmName());
	ASSERT_EQ(14, dynasty.getCoA().first);
}
