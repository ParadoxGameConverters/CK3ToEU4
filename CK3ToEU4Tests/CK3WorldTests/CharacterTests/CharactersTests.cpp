#include "../../CK3toEU4/Source/CK3World/Characters/Character.h"
#include "../../CK3toEU4/Source/CK3World/Characters/CharacterDomain.h"
#include "../../CK3toEU4/Source/CK3World/Characters/Characters.h"
#include "../../CK3toEU4/Source/CK3World/Cultures/Culture.h"
#include "../../CK3toEU4/Source/CK3World/Cultures/Cultures.h"
#include "../../CK3toEU4/Source/CK3World/Dynasties/House.h"
#include "../../CK3toEU4/Source/CK3World/Dynasties/Houses.h"
#include "../../CK3toEU4/Source/CK3World/Religions/Faith.h"
#include "../../CK3toEU4/Source/CK3World/Religions/Faiths.h"
#include "../../CK3toEU4/Source/CK3World/Titles/Title.h"
#include "../../CK3toEU4/Source/CK3World/Titles/Titles.h"
#include "../../CK3toEU4/Source/Mappers/TraitScraper/TraitScraper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_CharactersTests, CharactersDefaultToEmpty)
{
	std::stringstream input;
	CK3::Characters characters;
	characters.loadCharacters(input);

	ASSERT_TRUE(characters.getCharacters().empty());
}

TEST(CK3World_CharactersTests, CharactersCanBeLoaded)
{
	std::stringstream input;
	input << "11={}\n";
	input << "13={first_name=\"bob\"}\n";
	input << "15={first_name=\"alice\"}\n";

	CK3::Characters characters;
	characters.loadCharacters(input);
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
	CK3::Cultures cultures(input);

	std::stringstream input2;
	input2 << "1={culture = 15}\n";
	input2 << "2={culture = 13}\n";
	CK3::Characters characters;
	characters.loadCharacters(input2);
	characters.linkCultures(cultures);

	const auto& c1 = characters.getCharacters().find(1);
	const auto& c2 = characters.getCharacters().find(2);

	ASSERT_EQ("kru", *c1->second->getCulture()->second->getTemplate());
	ASSERT_EQ("akan", *c2->second->getCulture()->second->getTemplate());
}

TEST(CK3World_CharactersTests, linkingMissingCultureThrowsException)
{
	std::stringstream input;
	input << "13={culture_template=\"akan\"}\n";
	const CK3::Cultures cultures(input);

	std::stringstream input2;
	input2 << "1={culture = 13}\n";
	input2 << "2={culture = 15}\n";
	CK3::Characters characters;
	characters.loadCharacters(input2);

	ASSERT_THROW(characters.linkCultures(cultures), std::runtime_error);
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
	CK3::Characters characters;
	characters.loadCharacters(input2);
	characters.linkFaiths(faiths);

	const auto& c1 = characters.getCharacters().find(1);
	const auto& c2 = characters.getCharacters().find(2);

	ASSERT_EQ("theravada", c1->second->getFaith()->second->getName());
	ASSERT_EQ("old_bon", c2->second->getFaith()->second->getName());
}

TEST(CK3World_CharactersTests, linkingMissingFaithThrowsException)
{
	std::stringstream input;
	input << "15={tag=\"theravada\"}\n";
	const CK3::Faiths faiths(input);

	std::stringstream input2;
	input2 << "1 = { faith = 15 }\n";
	input2 << "2 = { faith = 13 }\n";
	CK3::Characters characters;
	characters.loadCharacters(input2);

	ASSERT_THROW(characters.linkFaiths(faiths), std::runtime_error);
}

TEST(CK3World_CharactersTests, housesCanBeLinked)
{
	std::stringstream input;
	input << "13={name=\"dynn_Villeneuve\"}\n";
	input << "15={name=\"dynn_Fournier\"}\n";
	const CK3::Houses houses(input);

	std::stringstream input2;
	input2 << "1={dynasty_house = 13}\n";
	input2 << "2={dynasty_house = 15}\n";
	CK3::Characters characters;
	characters.loadCharacters(input2);
	characters.linkHouses(houses);

	const auto& c1 = characters.getCharacters().find(1);
	const auto& c2 = characters.getCharacters().find(2);

	ASSERT_EQ("dynn_Villeneuve", c1->second->getHouse().second->getName());
	ASSERT_EQ("dynn_Fournier", c2->second->getHouse().second->getName());
}

TEST(CK3World_CharactersTests, linkingMissingHouseThrowsException)
{
	std::stringstream input;
	input << "13={name=\"dynn_Villeneuve\"}\n";
	const CK3::Houses houses(input);

	std::stringstream input2;
	input2 << "1={dynasty_house = 13}\n";
	input2 << "2={dynasty_house = 15}\n";
	CK3::Characters characters;
	characters.loadCharacters(input2);

	ASSERT_THROW(characters.linkHouses(houses), std::runtime_error);
}

TEST(CK3World_CharactersTests, houseLinkingFixesMissingFaithAndCulture)
{
	std::stringstream input;
	input << "13={name=\"dynn_Villeneuve\" head_of_house=2}\n";
	CK3::Houses houses(input);

	std::stringstream input2;
	input2 << "1={dynasty_house = 13}\n"; // <- No faith/culture
	input2 << "2={dynasty_house = 13 faith=9 culture=8}\n";
	CK3::Characters characters;
	characters.loadCharacters(input2);
	houses.linkCharacters(characters); // loads the house head
	characters.linkHouses(houses);

	const auto& c1 = characters.getCharacters().find(1);

	ASSERT_EQ(9, c1->second->getFaith()->first);
	ASSERT_EQ(8, c1->second->getCulture()->first);
}


TEST(CK3World_CharactersTests, titlesCanBeLinked)
{
	std::stringstream input;
	input << "1 = { key=c_county1 }\n";
	input << "2 = { key=c_county2 }\n";
	input << "3 = { key=c_county3 }\n";
	input << "4 = { key=c_county4 }\n";
	input << "5 = { key=c_county5 }\n";
	const CK3::Titles titles(input);

	std::stringstream input2;
	input2 << "100 = {\n";
	input2 << "\tclaim = { { title = 1 } { title = 3 } { title = 5 } }\n";
	input2 << "\tlanded_data = {\n";
	input2 << "\t\trealm_capital = 2\n";
	input2 << "\t\tdomain = { 4 2 }";
	input2 << "\t}";
	input2 << "}\n";
	CK3::Characters characters;
	characters.loadCharacters(input2);
	characters.linkTitles(titles);

	const auto& c1 = characters.getCharacters().find(100);

	ASSERT_EQ(3, c1->second->getClaims().size());
	ASSERT_EQ("c_county1", c1->second->getClaims().find(1)->second->getName());
	ASSERT_EQ("c_county3", c1->second->getClaims().find(3)->second->getName());
	ASSERT_EQ("c_county5", c1->second->getClaims().find(5)->second->getName());
	ASSERT_EQ("c_county2", c1->second->getCharacterDomain()->getRealmCapital().second->getName());
	ASSERT_EQ(2, c1->second->getCharacterDomain()->getDomain().size());
	ASSERT_EQ("c_county4", c1->second->getCharacterDomain()->getDomain()[0].second->getName());
	ASSERT_EQ("c_county2", c1->second->getCharacterDomain()->getDomain()[1].second->getName());
}

TEST(CK3World_CharactersTests, titlesLinkMissingClaimDropsErrantClaim)
{
	std::stringstream input;
	input << "1 = { key=c_county1 }\n";
	input << "2 = { key=c_county2 }\n";
	input << "3 = { key=c_county3 }\n";
	input << "4 = { key=c_county4 }\n";
	input << "5 = { key=c_county5 }\n";
	const CK3::Titles titles(input);

	std::stringstream input2;
	input2 << "100 = {\n";
	input2 << "\tclaim = { { title = 1 } { title = 3 } { title = 6 } }\n"; // missing 6
	input2 << "\tlanded_data = {\n";
	input2 << "\t\trealm_capital = 2\n";
	input2 << "\t\tdomain = { 4 2 }";
	input2 << "\t}";
	input2 << "}\n";
	CK3::Characters characters;
	characters.loadCharacters(input2);
	characters.linkTitles(titles);

	const auto& c1 = characters.getCharacters().find(100);

	ASSERT_EQ(2, c1->second->getClaims().size());
	ASSERT_EQ("c_county1", c1->second->getClaims().find(1)->second->getName());
	ASSERT_EQ("c_county3", c1->second->getClaims().find(3)->second->getName());
	ASSERT_FALSE(c1->second->getClaims().contains(6));
}

TEST(CK3World_CharactersTests, titlesLinkMissingCapitalIgnoresCapitalLinking)
{
	std::stringstream input;
	input << "1 = { key=c_county1 }\n";
	input << "2 = { key=c_county2 }\n";
	input << "3 = { key=c_county3 }\n";
	input << "4 = { key=c_county4 }\n";
	input << "5 = { key=c_county5 }\n";
	const CK3::Titles titles(input);

	std::stringstream input2;
	input2 << "100 = {\n";
	input2 << "\tclaim = { { title = 1 } { title = 3 } { title = 5 } }\n";
	input2 << "\tlanded_data = {\n";
	input2 << "\t\trealm_capital = 6\n"; // missing 6
	input2 << "\t\tdomain = { 4 2 }";
	input2 << "\t}";
	input2 << "}\n";
	CK3::Characters characters;
	characters.loadCharacters(input2);
	characters.linkTitles(titles);

	const auto& c1 = characters.getCharacters().find(100);

	ASSERT_EQ(6, c1->second->getCharacterDomain()->getRealmCapital().first);
	ASSERT_FALSE(c1->second->getCharacterDomain()->getRealmCapital().second);
}

TEST(CK3World_CharactersTests, titlesLinkMissingDomainThrowsException)
{
	std::stringstream input;
	input << "1 = { key=c_county1 }\n";
	input << "2 = { key=c_county2 }\n";
	input << "3 = { key=c_county3 }\n";
	input << "4 = { key=c_county4 }\n";
	input << "5 = { key=c_county5 }\n";
	const CK3::Titles titles(input);

	std::stringstream input2;
	input2 << "100 = {\n";
	input2 << "\tclaim = { { title = 1 } { title = 3 } { title = 5 } }\n";
	input2 << "\tlanded_data = {\n";
	input2 << "\t\trealm_capital = 2\n";
	input2 << "\t\tdomain = { 4 2 6 }"; // missing 6
	input2 << "\t}";
	input2 << "}\n";
	CK3::Characters characters;
	characters.loadCharacters(input2);

	ASSERT_THROW(characters.linkTitles(titles), std::runtime_error);
}


TEST(CK3World_CharactersTests, charactersCanBeLinked)
{
	std::stringstream input;
	input << "1 = {\n";
	input << "\tfirst_name = Alice\n";
	input << "\tcourt_data = { employer = 9 }\n";
	input << "\tfamily_data = { primary_spouse = 2 }\n";
	input << "}\n";
	input << "2 = {\n";
	input << "\tfirst_name = Bob\n";
	input << "\tcourt_data = { employer = 9 }\n";
	input << "\tfamily_data = { primary_spouse = 1 }\n";
	input << "}\n";
	input << "9 = {\n";
	input << "\tfirst_name = Carol\n";
	input << "}\n";
	CK3::Characters characters;
	characters.loadCharacters(input);
	characters.linkCharacters();

	const auto& c1 = characters.getCharacters().find(1);
	const auto& c2 = characters.getCharacters().find(2);

	ASSERT_EQ("Carol", c1->second->getEmployer()->second->getName());
	ASSERT_EQ("Carol", c2->second->getEmployer()->second->getName());
	ASSERT_EQ("Bob", c1->second->getSpouse()->second->getName());
	ASSERT_EQ("Alice", c2->second->getSpouse()->second->getName());
}

TEST(CK3World_CharactersTests, charactersLinkMissingSpouseResetsSpouse)
{
	std::stringstream input;
	input << "1 = {\n";
	input << "\tfirst_name = Alice\n";
	input << "\tcourt_data = { employer = 9 }\n";
	input << "\tfamily_data = { primary_spouse = 2 }\n";
	input << "}\n";
	input << "9 = {\n";
	input << "\tfirst_name = Carol\n";
	input << "}\n";
	CK3::Characters characters;
	characters.loadCharacters(input);
	characters.linkCharacters();

	const auto& c1 = characters.getCharacters().find(1);
	ASSERT_FALSE(c1->second->getSpouse());
}

TEST(CK3World_CharactersTests, charactersLinkMissingEmployerResetsEmployer)
{
	std::stringstream input;
	input << "1 = {\n";
	input << "\tfirst_name = Alice\n";
	input << "\tcourt_data = { employer = 9 }\n";
	input << "\tfamily_data = { primary_spouse = 2 }\n";
	input << "}\n";
	input << "2 = {\n";
	input << "\tfirst_name = Bob\n";
	input << "\tcourt_data = { employer = 9 }\n";
	input << "\tfamily_data = { primary_spouse = 1 }\n";
	input << "}\n";
	CK3::Characters characters;
	characters.loadCharacters(input);
	characters.linkCharacters();

	const auto& c1 = characters.getCharacters().find(1);
	ASSERT_FALSE(c1->second->getEmployer());
}

TEST(CK3World_CharactersTests, traitsCanBeLinked)
{
	std::stringstream input;
	input << "1 = {\n";
	input << "\tfirst_name = Alice\n";
	input << "\ttraits = { 1 2 3 }\n";
	input << "}\n";
	input << "2 = {\n";
	input << "\tfirst_name = Bob\n";
	input << "\ttraits = { 2 3 }\n";
	input << "}\n";
	input << "3 = {\n";
	input << "\tfirst_name = Carol\n";
	input << "}\n";
	CK3::Characters characters;
	characters.loadCharacters(input);
	characters.linkCharacters();

	std::stringstream input2;
	input2 << "trait1 = { index = 1 }\n";
	input2 << "trait2 = { index = 2 }\n";
	input2 << "trait3 = { index = 3 }\n";

	mappers::TraitScraper traitScraper;
	traitScraper.loadTraits(input2);

	characters.linkTraits(traitScraper);

	const auto& c1 = characters.getCharacters().find(1);
	const auto& c2 = characters.getCharacters().find(2);
	const auto& c3 = characters.getCharacters().find(3);

	ASSERT_EQ(3, c1->second->getTraits().size());
	ASSERT_EQ("trait1", c1->second->getTraits().find(1)->second);
	ASSERT_EQ("trait2", c1->second->getTraits().find(2)->second);
	ASSERT_EQ("trait3", c1->second->getTraits().find(3)->second);
	ASSERT_EQ(2, c2->second->getTraits().size());
	ASSERT_EQ("trait2", c2->second->getTraits().find(2)->second);
	ASSERT_EQ("trait3", c2->second->getTraits().find(3)->second);
	ASSERT_TRUE(c3->second->getTraits().empty());
}

TEST(CK3World_CharactersTests, traitsCanBePoked)
{
	std::stringstream input;
	input << "1 = {\n";
	input << "\tfirst_name = Alice\n";
	input << "\ttraits = { 1 2 3 }\n";
	input << "}\n";
	CK3::Characters characters;
	characters.loadCharacters(input);
	characters.linkCharacters();

	std::stringstream input2;
	input2 << "trait1 = { index = 1 }\n";
	input2 << "trait2 = { index = 2 }\n";
	input2 << "trait3 = { index = 3 }\n";

	mappers::TraitScraper traitScraper;
	traitScraper.loadTraits(input2);

	characters.linkTraits(traitScraper);

	const auto& c1 = characters.getCharacters().find(1);

	ASSERT_TRUE(c1->second->hasTrait("trait1"));
	ASSERT_FALSE(c1->second->hasTrait("trait-nonsense"));
}

TEST(CK3World_CharactersTests, traitsLinkMissingTraitsAreIgnored)
{
	std::stringstream input;
	input << "1 = {\n";
	input << "\tfirst_name = Alice\n";
	input << "\ttraits = { 1 2 3 4 9 }\n";
	input << "}\n";
	CK3::Characters characters;
	characters.loadCharacters(input);
	characters.linkCharacters();

	std::stringstream input2;
	input2 << "trait1 = { index = 1 }\n";
	input2 << "trait2 = { index = 2 }\n";
	input2 << "trait3 = { index = 3 }\n";

	mappers::TraitScraper traitScraper;
	traitScraper.loadTraits(input2);

	characters.linkTraits(traitScraper);

	const auto& c1 = characters.getCharacters().find(1);

	ASSERT_EQ(3, c1->second->getTraits().size());
	ASSERT_EQ("trait1", c1->second->getTraits().find(1)->second);
	ASSERT_EQ("trait2", c1->second->getTraits().find(2)->second);
	ASSERT_EQ("trait3", c1->second->getTraits().find(3)->second);
}
