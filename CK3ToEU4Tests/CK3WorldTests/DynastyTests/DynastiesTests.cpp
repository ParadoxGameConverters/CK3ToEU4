#include "../../CK3toEU4/Source/CK3World/Dynasties/Dynasty.h"
#include "../../CK3toEU4/Source/CK3World/Dynasties/Dynasties.h"
#include "../../CK3toEU4/Source/CK3World/CoatsOfArms/CoatsOfArms.h"
#include "../../CK3toEU4/Source/CK3World/CoatsOfArms/CoatOfArms.h"
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

TEST(CK3World_DynastiesTests, coatsCanBeLinked)
{
	std::stringstream input;
	input << "13={coat_of_arms_id = 1}\n";
	input << "15={coat_of_arms_id = 2}\n";
	CK3::Dynasties dynasties(input);

	std::stringstream input2;
	input2 << "1 = { pattern=\"smooth\"}\n";
	input2 << "2 = { pattern=\"lumpy\"}\n";
	const CK3::CoatsOfArms coats(input2);
	dynasties.linkCoats(coats);

	const auto& d1 = dynasties.getDynasties().find(13);
	const auto& d2 = dynasties.getDynasties().find(15);

	ASSERT_EQ("smooth", d1->second->getCoA()->second->getPattern());
	ASSERT_EQ("lumpy", d2->second->getCoA()->second->getPattern());
}

TEST(CK3World_DynastiesTests, linkingMissingCoatsThrowsException)
{
	std::stringstream input;
	input << "13={coat_of_arms_id = 1}\n";
	input << "15={coat_of_arms_id = 2}\n";
	CK3::Dynasties dynasties(input);

	std::stringstream input2;
	input2 << "1 = { pattern=\"smooth\"}\n";
	CK3::CoatsOfArms coats(input2);

	ASSERT_THROW(dynasties.linkCoats(coats), std::runtime_error);
}
