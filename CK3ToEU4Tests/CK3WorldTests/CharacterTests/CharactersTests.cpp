#include "../../CK3toEU4/Source/CK3World/Characters/Character.h"
#include "../../CK3toEU4/Source/CK3World/Characters/Characters.h"
#include "../../CK3toEU4/Source/CK3World/Cultures/Cultures.h"
#include "../../CK3toEU4/Source/CK3World/Cultures/Culture.h"
#include "../../CK3toEU4/Source/CK3World/Religions/Faith.h"
#include "../../CK3toEU4/Source/CK3World/Religions/Faiths.h"

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

TEST(CK3World_CharactersTests, culturesCanBeLinked)
{
	std::stringstream input;
	input << "13={culture_template=\"akan\"}\n";
	input << "15={culture_template=\"kru\"}\n";
	const CK3::Cultures cultures(input);

	std::stringstream input2;
	input2 << "1={culture = 15}\n";
	input2 << "2={culture = 13}\n";
	CK3::Characters characters(input2);
	characters.loadCultures(cultures);

	const auto& c1 = characters.getCharacters().find(1);
	const auto& c2 = characters.getCharacters().find(2);

	ASSERT_EQ("kru", c1->second->getCulture().second->getName());
	ASSERT_EQ("akan", c2->second->getCulture().second->getName());
}

TEST(CK3World_CharactersTests, linkingMissingCultureThrowsException)
{
	std::stringstream input;
	input << "13={culture_template=\"akan\"}\n";
	const CK3::Cultures cultures(input);

	std::stringstream input2;
	input2 << "1={culture = 13}\n";
	input2 << "2={culture = 15}\n";
	CK3::Characters characters(input2);	

	ASSERT_THROW(characters.loadCultures(cultures);, std::runtime_error);
}

TEST(CK3World_CharactersTests, faithsCanBeLinked)
{
	std::stringstream input;
	input << "13={tag=\"old_bon\"}\n";
	input << "15={tag=\"theravada\"}\n";
	const CK3::Faiths faiths(input);

	std::stringstream input2;
	input2 << "1={faith = 15}\n";
	input2 << "2={faith = 13}\n";
	CK3::Characters characters(input2);
	characters.loadFaiths(faiths);

	const auto& c1 = characters.getCharacters().find(1);
	const auto& c2 = characters.getCharacters().find(2);

	ASSERT_EQ("theravada", c1->second->getFaith().second->getName());
	ASSERT_EQ("old_bon", c2->second->getFaith().second->getName());
}

TEST(CK3World_CharactersTests, linkingMissingFaithThrowsException)
{
	std::stringstream input;
	input << "15={tag=\"theravada\"}\n";
	const CK3::Faiths faiths(input);

	std::stringstream input2;
	input2 << "1 = { faith = 15 }\n";
	input2 << "2 = { faith = 13 }\n";
	CK3::Characters characters(input2);

	ASSERT_THROW(characters.loadFaiths(faiths);, std::runtime_error);
}
