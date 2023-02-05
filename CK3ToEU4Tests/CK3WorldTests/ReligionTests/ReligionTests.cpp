#include "../CK3ToEU4/Source/CK3World/Religions/Religion.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_ReligionTests, religionIDLoads)
{
	std::stringstream input;
	const CK3::Religion religion(input, 42);

	ASSERT_EQ(42, religion.getID());
}

TEST(CK3World_ReligionTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::Religion religion(input, 1);

	ASSERT_TRUE(religion.getName().empty());
	ASSERT_TRUE(religion.getFamily().empty());
	ASSERT_TRUE(religion.getFaiths().empty());
}

TEST(CK3World_ReligionTests, religionPrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "tag = \"christianity_religion\"\n";
	input << "family = \"rf_abrahamic\"\n";
	input << "faiths={ 9 10 11 12 13 }\n";

	const CK3::Religion religion(input, 42);

	ASSERT_EQ("christianity_religion", religion.getName());
	ASSERT_EQ("rf_abrahamic", religion.getFamily());
	ASSERT_EQ(5, religion.getFaiths().size());
}
