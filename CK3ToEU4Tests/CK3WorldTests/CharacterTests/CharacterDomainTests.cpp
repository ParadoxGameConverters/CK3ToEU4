#include "../CK3toEU4/Source/CK3World/Characters/CharacterDomain.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_CharacterDomainTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::CharacterDomain domain(input);

	ASSERT_TRUE(domain.getDomain().empty());
	ASSERT_TRUE(domain.getLaws().empty());
	ASSERT_TRUE(domain.getGovernment().empty());
	ASSERT_FALSE(domain.isPowerfulVassal());
	ASSERT_FALSE(domain.getVassalStrength());
	ASSERT_FALSE(domain.getRealmCapital().first);
}

TEST(CK3World_CharacterDomainTests, domainPrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "domain = { 1 2 3 }\n";
	input << "laws = {\"law_1\" \"law_2\" \"law_3\" \"law_4\" \"law_4\" \"law_4\"}\n";
	input << "is_powerful_vassal = yes\n";
	input << "vassal_power_value = 1234.56\n";
	input << "government = \"the_government\"\n";
	input << "realm_capital = 1234\n";
	input << "}";

	const CK3::CharacterDomain domain(input);

	ASSERT_EQ(3, domain.getDomain().size());
	ASSERT_EQ(4, domain.getLaws().size());
	ASSERT_TRUE(domain.isPowerfulVassal());
	ASSERT_NEAR(1234.56, domain.getVassalStrength(), 0.01);
	ASSERT_EQ("the_government", domain.getGovernment());
	ASSERT_EQ(1234, domain.getRealmCapital().first);
}
