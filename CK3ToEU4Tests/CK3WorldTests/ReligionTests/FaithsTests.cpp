#include "../../CK3toEU4/Source/CK3World/Religions/Faith.h"
#include "../../CK3toEU4/Source/CK3World/Religions/Faiths.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_FaithsTests, FaithsDefaultToEmpty)
{
	std::stringstream input;
	const CK3::Faiths faiths(input);

	ASSERT_TRUE(faiths.getFaiths().empty());
}

TEST(CK3World_FaithsTests, FaithsCanBeLoaded)
{
	std::stringstream input;
	input << "13={tag=\"old_bon\"}\n";
	input << "15={tag=\"theravada\"}\n";

	const CK3::Faiths faiths(input);
	const auto& h1 = faiths.getFaiths().find(13);
	const auto& h2 = faiths.getFaiths().find(15);

	ASSERT_EQ("old_bon", h1->second->getName());
	ASSERT_EQ("theravada", h2->second->getName());
}
