#include "../../CK3toEU4/Source/CK3World/Characters/Character.h"
#include "../../CK3toEU4/Source/CK3World/Characters/Characters.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_CharactersTests, CharactersDefaultToEmpty)
{
	std::stringstream input;
	const CK3::Characters characters(input);

	ASSERT_TRUE(characters.getCharacters().empty());
}

TEST(CK3World_CharactersTests, CharactersCanBeLoaded)
{
	std::stringstream input;
	input << "11={}\n";
	input << "13={first_name=\"bob\"}\n";
	input << "15={first_name=\"alice\"}\n";

	const CK3::Characters characters(input);
	const auto& b1 = characters.getCharacters().find(11);
	const auto& b2 = characters.getCharacters().find(13);
	const auto& b3 = characters.getCharacters().find(15);

	ASSERT_TRUE(b1->second->getClaims().empty());
	ASSERT_EQ("bob", b2->second->getName());
	ASSERT_EQ("alice", b3->second->getName());
}
