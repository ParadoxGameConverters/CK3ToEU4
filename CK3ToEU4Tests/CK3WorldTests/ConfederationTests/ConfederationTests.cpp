#include "../CK3ToEU4/Source/CK3World/Confederations/Confederation.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

using testing::ElementsAre;
TEST(CK3World_ConfederationTests, cultureIDLoads)
{
	std::stringstream input;
	const CK3::Confederation confederation(input, 42);

	EXPECT_EQ(42, confederation.getID());
}

TEST(CK3World_ConfederationTests, loadValuesDefaultToDefaults)
{
	std::stringstream input;
	const CK3::Confederation confederation(input, 1);

	EXPECT_EQ(1, confederation.getID());
	EXPECT_TRUE(confederation.getName().empty());
	EXPECT_FALSE(confederation.getColor());
	EXPECT_FALSE(confederation.getCoat().first);
	EXPECT_FALSE(confederation.getCoat().second);
	EXPECT_TRUE(confederation.getMembers().empty());
}

TEST(CK3World_ConfederationTests, confederationPrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "members={ 16793418 16795222 16810363 26726 16799741 }\n";
	input << "name=\"Polabian Confederation\"\n";
	input << "color=rgb { 80 110 80 }\n";
	input << "coat_of_arms=20636\n";

	const CK3::Confederation confederation(input, 42);

	EXPECT_THAT(confederation.getMembers(),
		 ElementsAre(std::pair<long long, std::shared_ptr<CK3::Character>>(26726, nullptr),
			  std::pair<long long, std::shared_ptr<CK3::Character>>(16793418, nullptr),
			  std::pair<long long, std::shared_ptr<CK3::Character>>(16795222, nullptr),
			  std::pair<long long, std::shared_ptr<CK3::Character>>(16799741, nullptr),
			  std::pair<long long, std::shared_ptr<CK3::Character>>(16810363, nullptr)));
	EXPECT_EQ("Polabian Confederation", confederation.getName());
	EXPECT_EQ("= rgb { 80 110 80 }", confederation.getColor()->outputRgb());
	EXPECT_EQ(20636, confederation.getCoat().first);
	EXPECT_FALSE(confederation.getCoat().second);
}
