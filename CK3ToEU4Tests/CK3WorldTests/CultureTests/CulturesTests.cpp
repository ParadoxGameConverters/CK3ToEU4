#include "../../CK3toEU4/Source/CK3World/Cultures/Culture.h"
#include "../../CK3toEU4/Source/CK3World/Cultures/Cultures.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_CulturesTests, CulturesDefaultToEmpty)
{
	std::stringstream input;
	const CK3::Cultures cultures(input);

	ASSERT_TRUE(cultures.getCultures().empty());
}

TEST(CK3World_CulturesTests, UnbundledCulturesCanBeLoaded)
{
	std::stringstream input;
	input << "13={culture_template=\"akan\"}\n";
	input << "15={culture_template=\"kru\"}\n";

	const CK3::Cultures cultures(input);
	const auto& c1 = cultures.getCultures().find(13);
	const auto& c2 = cultures.getCultures().find(15);

	ASSERT_EQ(2, cultures.getCultures().size());
	ASSERT_EQ("akan", c1->second->getTemplate());
	ASSERT_EQ("kru", c2->second->getTemplate());
}

TEST(CK3World_CulturesTests, BundledCulturesCanBeLoaded)
{
	std::stringstream input;
	input << "cultures={\n";
	input << "\t13={culture_template=\"akan\"}\n";
	input << "\t15={culture_template=\"kru\"}\n";
	input << "}\n";

	const CK3::Cultures cultures(input);
	const auto& c1 = cultures.getCultures().find(13);
	const auto& c2 = cultures.getCultures().find(15);

	ASSERT_EQ(2, cultures.getCultures().size());
	ASSERT_EQ("akan", c1->second->getTemplate());
	ASSERT_EQ("kru", c2->second->getTemplate());
}
