#include "../CK3toEU4/Source/CK3World/CoatsOfArms/EmblemInstance.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_CharacterDomainTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::EmblemInstance emblemInstance(input);

	ASSERT_TRUE(emblemInstance.getDomain().empty());
	ASSERT_TRUE(emblemInstance.getLaws().empty());
	ASSERT_TRUE(emblemInstance.getGovernment().empty());
	ASSERT_FALSE(emblemInstance.isPowerfulVassal());
	ASSERT_FALSE(emblemInstance.getVassalStrength());
	ASSERT_FALSE(emblemInstance.getRealmCapital().first);
}

TEST(CK3World_CharacterDomainTests, domainPrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "emblemInstance = { 1 2 3 }\n";
	input << "laws = {\"law_1\" \"law_2\" \"law_3\" \"law_4\" \"law_4\" \"law_4\"}\n";
	input << "is_powerful_vassal = yes\n";
	input << "vassal_power_value = 1234.56\n";
	input << "government = \"the_government\"\n";
	input << "realm_capital = 1234\n";

	const CK3::EmblemInstance emblemInstance(input);

	ASSERT_EQ(3, emblemInstance.getDomain().size());
	ASSERT_EQ(4, emblemInstance.getLaws().size());
	ASSERT_TRUE(emblemInstance.isPowerfulVassal());
	ASSERT_NEAR(1234.56, emblemInstance.getVassalStrength(), 0.01);
	ASSERT_EQ("the_government", emblemInstance.getGovernment());
	ASSERT_EQ(1234, emblemInstance.getRealmCapital().first);
}
