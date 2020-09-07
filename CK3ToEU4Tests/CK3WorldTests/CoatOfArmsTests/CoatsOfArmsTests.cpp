#include "../../CK3toEU4/Source/CK3World/CoatsOfArms/CoatsOfArms.h"
#include "../../CK3toEU4/Source/CK3World/CoatsOfArms/CoatOfArms.h"
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
	const auto& d1 = coatsOfArms.getCoats().find(13);
	const auto& d2 = coatsOfArms.getCoats().find(15);

	ASSERT_EQ(2, coatsOfArms.getCoats().size());
	ASSERT_EQ("k_france", d1->second->getParent().first);
	ASSERT_EQ("k_england", d2->second->getParent().first);
}

TEST(CK3World_CoatsOfArmsTests, UnBundledCoatsOfArmsCanBeLoaded)
{
	std::stringstream input;
	input << "13={parent=\"k_france\"}\n";
	input << "15={parent=\"k_england\"}\n";

	const CK3::CoatsOfArms coatsOfArms(input);
	const auto& d1 = coatsOfArms.getCoats().find(13);
	const auto& d2 = coatsOfArms.getCoats().find(15);

	ASSERT_EQ(2, coatsOfArms.getCoats().size());
	ASSERT_EQ("k_france", d1->second->getParent().first);
	ASSERT_EQ("k_england", d2->second->getParent().first);
}
