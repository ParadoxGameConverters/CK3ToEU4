#include "../../CK3toEU4/Source/CK3World/Characters/Character.h"
#include "../../CK3toEU4/Source/CK3World/Characters/Characters.h"
#include "../../CK3toEU4/Source/CK3World/CoatsOfArms/CoatOfArms.h"
#include "../../CK3toEU4/Source/CK3World/CoatsOfArms/CoatsOfArms.h"
#include "../../CK3toEU4/Source/CK3World/Geography/ProvinceHolding.h"
#include "../../CK3toEU4/Source/CK3World/Titles/LandedTitles.h"
#include "../../CK3toEU4/Source/CK3World/Titles/Title.h"
#include "../../CK3toEU4/Source/CK3World/Titles/Titles.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_TitlesTests, titlesDefaultToEmpty)
{
	std::stringstream input;
	const CK3::Titles titles(input);

	ASSERT_TRUE(titles.getTitles().empty());
}

TEST(CK3World_TitlesTests, titlesCanBeLoaded)
{
	std::stringstream input;
	input << "123={key=c_roma}\n";
	input << "12345={key=c_roma2}\n";

	const CK3::Titles titles(input);
	const auto& roma = titles.getTitles().find("c_roma");
	const auto& roma2 = titles.getTitles().find("c_roma2");

	ASSERT_EQ("c_roma", roma->second->getName());
	ASSERT_EQ(123, roma->second->getID());
	ASSERT_EQ("c_roma2", roma2->second->getName());
	ASSERT_EQ(12345, roma2->second->getID());
}

TEST(CK3World_TitlesTests, namelessTitlesAreIgnored)
{
	std::stringstream input;
	input << "123={key=c_roma}\n";
	input << "1234={}\n";
	input << "12345={key=c_roma2}\n";

	const CK3::Titles titles(input);

	ASSERT_EQ(2, titles.getTitles().size());
}

TEST(CK3World_TitlesTests, insaneTitlesThrowException)
{
	std::stringstream input;
	input << "123={key=c_roma}\n";
	input << "1234={}\n";
	input << "1234512356789012345678901234567890={key=c_roma2}\n";

	try
	{
		const CK3::Titles titles(input);
		FAIL();
	}
	catch (const std::runtime_error& e)
	{
		ASSERT_STREQ("Cannot import title ID: 1234512356789012345678901234567890 (stoll argument out of range)", e.what());
	}
}

TEST(CK3World_TitlesTests, junkTitlesAreIgnored)
{
	std::stringstream input;
	input << "123={key=c_roma}\n";
	input << "1234=none\n";
	input << "12345={key=c_roma2}\n";

	const CK3::Titles titles(input);

	ASSERT_EQ(2, titles.getTitles().size());
}

TEST(CK3World_TitlesTests, coatsCanBeLinked)
{
	std::stringstream input;
	input << "13={key=\"c_county\"\n coat_of_arms_id = 1}\n";
	input << "15={key=\"d_duchy\"\n coat_of_arms_id = 2}\n";
	CK3::Titles titles(input);

	std::stringstream input2;
	input2 << "1 = { pattern=\"smooth\"}\n";
	input2 << "2 = { pattern=\"lumpy\"}\n";
	const CK3::CoatsOfArms coats(input2);
	titles.linkCoats(coats);

	const auto& t1 = titles.getTitles().find("c_county");
	const auto& t2 = titles.getTitles().find("d_duchy");

	ASSERT_EQ("smooth", t1->second->getCoA()->second->getPattern());
	ASSERT_EQ("lumpy", t2->second->getCoA()->second->getPattern());
}

TEST(CK3World_TitlesTests, linkingMissingCoatsThrowsException)
{
	std::stringstream input;
	input << "13={key=\"c_county\"\n coat_of_arms_id = 1}\n";
	input << "15={key=\"d_duchy\"\n coat_of_arms_id = 2}\n";
	CK3::Titles titles(input);

	std::stringstream input2;
	input2 << "1 = { pattern=\"smooth\"}\n";
	CK3::CoatsOfArms coats(input2);

	ASSERT_THROW(titles.linkCoats(coats), std::runtime_error);
}

TEST(CK3World_TitlesTests, titlesCanBeLinked)
{
	std::stringstream input;
	input << "1 = { key = c_county1 holder = 1 de_facto_liege = 6 de_jure_liege = 6 }\n";
	input << "2 = { key = c_county2 holder = 2 de_facto_liege = 6 de_jure_liege = 6  }\n";
	input << "3 = { key = c_county3 holder = 3 de_facto_liege = 6 de_jure_liege = 7  }\n";
	input << "4 = { key = c_county4 holder = 4 de_facto_liege = 7 de_jure_liege = 7  }\n";
	input << "5 = { key = c_county5 holder = 5 de_facto_liege = 7 de_jure_liege = 7  }\n";
	input << "6 = { key = d_duchy1 holder = 6 de_facto_liege = 8 de_jure_liege = 8 de_jure_vassals = { 1 2 } }\n";
	input << "7 = { key = d_duchy2 holder = 7 de_jure_liege = 8  de_jure_vassals = { 3 4 5 } }\n";
	input << "8 = { key = k_kingdom1 holder = 8 de_jure_vassals = { 6 7 } }\n";
	CK3::Titles titles(input);

	titles.linkTitles();

	const auto& t1 = titles.getTitles().find("c_county1");
	const auto& t2 = titles.getTitles().find("c_county2");
	const auto& t3 = titles.getTitles().find("c_county3");
	const auto& t4 = titles.getTitles().find("c_county4");
	const auto& t5 = titles.getTitles().find("c_county5");
	const auto& t6 = titles.getTitles().find("d_duchy1");
	const auto& t7 = titles.getTitles().find("d_duchy2");
	const auto& t8 = titles.getTitles().find("k_kingdom1");

	// Testing defacto liege
	ASSERT_EQ("d_duchy1", t1->second->getDFLiege()->second->getName());
	ASSERT_EQ("d_duchy1", t2->second->getDFLiege()->second->getName());
	ASSERT_EQ("d_duchy1", t3->second->getDFLiege()->second->getName());
	ASSERT_EQ("d_duchy2", t4->second->getDFLiege()->second->getName());
	ASSERT_EQ("d_duchy2", t5->second->getDFLiege()->second->getName());
	ASSERT_EQ("k_kingdom1", t6->second->getDFLiege()->second->getName());
	ASSERT_FALSE(t7->second->getDFLiege());
	ASSERT_FALSE(t8->second->getDFLiege());

	// Testing dejure liege
	ASSERT_EQ("d_duchy1", t1->second->getDJLiege()->second->getName());
	ASSERT_EQ("d_duchy1", t2->second->getDJLiege()->second->getName());
	ASSERT_EQ("d_duchy2", t3->second->getDJLiege()->second->getName());
	ASSERT_EQ("d_duchy2", t4->second->getDJLiege()->second->getName());
	ASSERT_EQ("d_duchy2", t5->second->getDJLiege()->second->getName());
	ASSERT_EQ("k_kingdom1", t6->second->getDJLiege()->second->getName());
	ASSERT_EQ("k_kingdom1", t7->second->getDJLiege()->second->getName());
	ASSERT_FALSE(t8->second->getDJLiege());

	// Testing de jure vassals
	ASSERT_TRUE(t1->second->getDJVassals().empty());
	ASSERT_TRUE(t2->second->getDJVassals().empty());
	ASSERT_TRUE(t3->second->getDJVassals().empty());
	ASSERT_TRUE(t4->second->getDJVassals().empty());
	ASSERT_TRUE(t5->second->getDJVassals().empty());
	ASSERT_EQ(2, t6->second->getDJVassals().size());
	ASSERT_EQ("c_county1", t6->second->getDJVassals().find(1)->second->getName());
	ASSERT_EQ("c_county2", t6->second->getDJVassals().find(2)->second->getName());
	ASSERT_EQ(3, t7->second->getDJVassals().size());
	ASSERT_EQ("c_county3", t7->second->getDJVassals().find(3)->second->getName());
	ASSERT_EQ("c_county4", t7->second->getDJVassals().find(4)->second->getName());
	ASSERT_EQ("c_county5", t7->second->getDJVassals().find(5)->second->getName());
	ASSERT_EQ(2, t8->second->getDJVassals().size());
	ASSERT_EQ("d_duchy1", t8->second->getDJVassals().find(6)->second->getName());
	ASSERT_EQ("d_duchy2", t8->second->getDJVassals().find(7)->second->getName());

	// Testing defacto vassals
	ASSERT_TRUE(t1->second->getDJVassals().empty());
	ASSERT_TRUE(t2->second->getDJVassals().empty());
	ASSERT_TRUE(t3->second->getDJVassals().empty());
	ASSERT_TRUE(t4->second->getDJVassals().empty());
	ASSERT_TRUE(t5->second->getDJVassals().empty());
	ASSERT_EQ(3, t6->second->getDFVassals().size());
	ASSERT_EQ("c_county1", t6->second->getDFVassals().find(1)->second->getName());
	ASSERT_EQ("c_county2", t6->second->getDFVassals().find(2)->second->getName());
	ASSERT_EQ("c_county3", t6->second->getDFVassals().find(3)->second->getName());
	ASSERT_EQ(2, t7->second->getDFVassals().size());
	ASSERT_EQ("c_county4", t7->second->getDFVassals().find(4)->second->getName());
	ASSERT_EQ("c_county5", t7->second->getDFVassals().find(5)->second->getName());
	ASSERT_EQ(1, t8->second->getDFVassals().size());
	ASSERT_EQ("d_duchy1", t8->second->getDFVassals().find(6)->second->getName());

	// Testing self-loading on counties.
	ASSERT_EQ("c_county1", t1->second->getOwnedDFCounties().find("c_county1")->second->getName());
	ASSERT_EQ("c_county1", t1->second->getOwnedDJCounties().find("c_county1")->second->getName());
	ASSERT_EQ("c_county2", t2->second->getOwnedDFCounties().find("c_county2")->second->getName());
	ASSERT_EQ("c_county2", t2->second->getOwnedDJCounties().find("c_county2")->second->getName());
	ASSERT_EQ("c_county3", t3->second->getOwnedDFCounties().find("c_county3")->second->getName());
	ASSERT_EQ("c_county3", t3->second->getOwnedDJCounties().find("c_county3")->second->getName());
	ASSERT_EQ("c_county4", t4->second->getOwnedDFCounties().find("c_county4")->second->getName());
	ASSERT_EQ("c_county4", t4->second->getOwnedDJCounties().find("c_county4")->second->getName());
	ASSERT_EQ("c_county5", t5->second->getOwnedDFCounties().find("c_county5")->second->getName());
	ASSERT_EQ("c_county5", t5->second->getOwnedDJCounties().find("c_county5")->second->getName());
}

TEST(CK3World_TitlesTests, titleLinkMissingDJLiegeThrowsException)
{
	std::stringstream input;
	input << "1 = { key = c_county1 de_facto_liege = 6 de_jure_liege = 6 }\n";
	input << "2 = { key = c_county2 de_facto_liege = 6 de_jure_liege = 6  }\n";
	input << "3 = { key = c_county3 de_facto_liege = 6 de_jure_liege = 7  }\n";
	input << "4 = { key = c_county4 de_facto_liege = 7 de_jure_liege = 7  }\n";
	input << "5 = { key = c_county5 de_facto_liege = 7 de_jure_liege = 7  }\n";
	input << "6 = { key = d_duchy1 de_facto_liege = 8 de_jure_liege = 9 de_jure_vassals = { 1 2 } }\n"; // There is no 9
	input << "7 = { key = d_duchy2 de_jure_liege = 8  de_jure_vassals = { 3 4 5 } }\n";
	input << "8 = { key = k_kingdom1 de_jure_vassals = { 6 7 } }\n";
	CK3::Titles titles(input);

	ASSERT_THROW(titles.linkTitles(), std::runtime_error);
}

TEST(CK3World_TitlesTests, titleLinkMissingDFLiegeFixesDFLiegeToNullopt)
{
	std::stringstream input;
	input << "1 = { key = c_county1 holder = 1 de_facto_liege = 6 de_jure_liege = 6 }\n";
	input << "2 = { key = c_county2 holder = 2 de_facto_liege = 6 de_jure_liege = 6  }\n";
	input << "3 = { key = c_county3 holder = 3 de_facto_liege = 6 de_jure_liege = 7  }\n";
	input << "4 = { key = c_county4 holder = 4 de_facto_liege = 7 de_jure_liege = 7  }\n";
	input << "5 = { key = c_county5 holder = 5 de_facto_liege = 7 de_jure_liege = 7  }\n";
	input << "6 = { key = d_duchy1 holder = 6 de_facto_liege = 9 de_jure_liege = 8 de_jure_vassals = { 1 2 } }\n"; // There is no 9
	input << "7 = { key = d_duchy2 holder = 7 de_jure_liege = 8  de_jure_vassals = { 3 4 5 } }\n";
	input << "8 = { key = k_kingdom1 holder = 8 de_jure_vassals = { 6 7 } }\n";
	CK3::Titles titles(input);
	titles.linkTitles();
	
	ASSERT_FALSE(titles.getTitles().find("d_duchy1")->second->getDFLiege());
}

TEST(CK3World_TitlesTests, titleLinkMissingDJVassalThrowsException)
{
	std::stringstream input;
	input << "1 = { key = c_county1 de_facto_liege = 6 de_jure_liege = 6 }\n";
	input << "2 = { key = c_county2 de_facto_liege = 6 de_jure_liege = 6  }\n";
	input << "3 = { key = c_county3 de_facto_liege = 6 de_jure_liege = 7  }\n";
	input << "4 = { key = c_county4 de_facto_liege = 7 de_jure_liege = 7  }\n";
	input << "5 = { key = c_county5 de_facto_liege = 7 de_jure_liege = 7  }\n";
	input << "6 = { key = d_duchy1 de_facto_liege = 8 de_jure_liege = 8 de_jure_vassals = { 1 2 9 } }\n"; // There is no 9
	input << "7 = { key = d_duchy2 de_jure_liege = 8  de_jure_vassals = { 3 4 5 } }\n";
	input << "8 = { key = k_kingdom1 de_jure_vassals = { 6 7 } }\n";
	CK3::Titles titles(input);

	ASSERT_THROW(titles.linkTitles(), std::runtime_error);
}

TEST(CK3World_TitlesTests, charactersCanBeLinked)
{
	std::stringstream input;
	input << "13 = {\n";
	input << "\tkey= c_county\n";
	input << "\tholder = 1\n";
	input << "\tclaim = {2 3}\n";
	input << "\their = {2}\n";
	input << "\thistory = {\n";
	input << "\t\t1.1.1 = 2\n";
	input << "\t\t2.2.2 = 1\n";
	input << "\t\t3.3.3 = { type = destroyed }\n";
	input << "\t}\n";
	input << "}\n";
	input << "15 = { key = d_duchy holder = 2 claim = { 1 } heir = { 3 1 } succession_election = { electors = { 2 3 } } }\n";
	CK3::Titles titles(input);

	std::stringstream input2;
	input2 << "1 = { first_name = Alice }\n";
	input2 << "2 = { first_name = Bob }\n";
	input2 << "3 = { first_name = Carol }\n";
	const CK3::Characters characters(input2);
	titles.linkCharacters(characters);

	const auto& t1 = titles.getTitles().find("c_county");
	const auto& t2 = titles.getTitles().find("d_duchy");

	ASSERT_EQ("Alice", t1->second->getHolder()->second->getName());
	ASSERT_EQ("Bob", t2->second->getHolder()->second->getName());
	ASSERT_EQ(2, t1->second->getClaimants().size());
	ASSERT_EQ("Bob", t1->second->getClaimants().find(2)->second->getName());
	ASSERT_EQ("Carol", t1->second->getClaimants().find(3)->second->getName());
	ASSERT_EQ(1, t2->second->getClaimants().size());
	ASSERT_EQ("Alice", t2->second->getClaimants().find(1)->second->getName());
	ASSERT_EQ(1, t1->second->getHeirs().size());
	ASSERT_EQ("Bob", t1->second->getHeirs()[0].second->getName());
	ASSERT_EQ(2, t2->second->getHeirs().size());
	ASSERT_EQ("Carol", t2->second->getHeirs()[0].second->getName());
	ASSERT_EQ("Alice", t2->second->getHeirs()[1].second->getName());
	ASSERT_EQ(2, t2->second->getElectors().size());
	ASSERT_EQ("Bob", t2->second->getElectors().find(2)->second->getName());
	ASSERT_EQ("Carol", t2->second->getElectors().find(3)->second->getName());
	ASSERT_EQ(2, t1->second->getPreviousHolders().size());
	ASSERT_EQ("Bob", t1->second->getPreviousHolders()[0].second->getName());
	ASSERT_EQ("Alice", t1->second->getPreviousHolders()[1].second->getName());
}

TEST(CK3World_TitlesTests, charactersLinkMissingHolderThrowsException)
{
	std::stringstream input;
	input << "13 = { key= c_county holder = 9 claim = { 2 3 } heir = { 2 } }\n"; // missing 9
	input << "15 = { key = d_duchy holder = 2 claim = { 1 } heir = { 3 1 } }\n";
	CK3::Titles titles(input);

	std::stringstream input2;
	input2 << "1 = { first_name = Alice }\n";
	input2 << "2 = { first_name = Bob }\n";
	input2 << "3 = { first_name = Carol }\n";
	const CK3::Characters characters(input2);

	ASSERT_THROW(titles.linkCharacters(characters), std::runtime_error);
}

TEST(CK3World_TitlesTests, charactersLinkMissingClaimantIgnoresClaimant)
{
	std::stringstream input;
	input << "13 = { key= c_county holder = 1 claim = { 9 3 } heir = { 2 } }\n"; // missing 9
	input << "15 = { key = d_duchy holder = 2 claim = { 1 } heir = { 3 1 } }\n";
	CK3::Titles titles(input);

	std::stringstream input2;
	input2 << "1 = { first_name = Alice }\n";
	input2 << "2 = { first_name = Bob }\n";
	input2 << "3 = { first_name = Carol }\n";
	const CK3::Characters characters(input2);
	titles.linkCharacters(characters);

	const auto& t1 = titles.getTitles().find("c_county");
	ASSERT_EQ(1, t1->second->getClaimants().size());	
}

TEST(CK3World_TitlesTests, charactersLinkMissingHeirDropsHeir)
{
	std::stringstream input;
	input << "13 = { key= c_county heir = { 9 3 } }\n"; // missing 9
	CK3::Titles titles(input);

	std::stringstream input2;
	input2 << "1 = { first_name = Alice }\n";
	input2 << "3 = { first_name = Carol }\n";
	const CK3::Characters characters(input2);
	titles.linkCharacters(characters);

	const auto& t1 = titles.getTitles().find("c_county");

	ASSERT_EQ(1, t1->second->getHeirs().size());
	ASSERT_EQ("Carol", t1->second->getHeirs()[0].second->getName());
}

TEST(CK3World_TitlesTests, landedTitlesCanBeLinked)
{
	std::stringstream input;
	input << "13 = { key= c_county }\n";
	input << "15 = { key = d_duchy }\n";
	input << "17 = { key = x_x_17 }\n"; // landless faction
	CK3::Titles titles(input);

	std::stringstream input2;
	input2 << "c_county = { landless = yes }\n";
	input2 << "d_duchy = { province = 12 }\n";
	CK3::LandedTitles clay;
	clay.loadTitles(input2);
	titles.linkLandedTitles(clay);

	const auto& t1 = titles.getTitles().find("c_county");
	const auto& t2 = titles.getTitles().find("d_duchy");
	const auto& t3 = titles.getTitles().find("x_x_17");

	ASSERT_TRUE(t1->second->getClay()->isLandless());
	ASSERT_EQ(12, t2->second->getClay()->getProvince()->first);
	ASSERT_FALSE(t3->second->getClay());
}

TEST(CK3World_TitlesTests, titlesCanBeCoalesced)
{
	std::stringstream input;
	input << "1 = { key = c_county1 holder = 1 de_facto_liege = 6 de_jure_liege = 6 }\n";
	input << "2 = { key = c_county2 holder = 2 de_facto_liege = 6 de_jure_liege = 6  }\n";
	input << "3 = { key = c_county3 holder = 3 de_facto_liege = 6 de_jure_liege = 7  }\n";
	input << "4 = { key = c_county4 holder = 4 de_facto_liege = 7 de_jure_liege = 7  }\n";
	input << "5 = { key = c_county5 holder = 5 de_facto_liege = 7 de_jure_liege = 7  }\n";
	input << "6 = { key = d_duchy1 holder = 6 de_facto_liege = 8 de_jure_liege = 8 de_jure_vassals = { 1 2 } }\n";
	input << "7 = { key = d_duchy2 holder = 7 de_jure_liege = 8  de_jure_vassals = { 3 4 5 } }\n";
	input << "8 = { key = k_kingdom1 holder = 8  de_jure_vassals = { 6 7 } }\n";
	CK3::Titles titles(input);
	titles.linkTitles();

	const auto& ownedDF8 = titles.getTitles().find("k_kingdom1")->second->coalesceDFCounties();
	const auto& ownedDJ8 = titles.getTitles().find("k_kingdom1")->second->coalesceDJCounties();

	ASSERT_EQ(5, ownedDJ8.size());
	ASSERT_EQ("c_county1", ownedDJ8.find("c_county1")->second->getName());
	ASSERT_EQ("c_county2", ownedDJ8.find("c_county2")->second->getName());
	ASSERT_EQ("c_county3", ownedDJ8.find("c_county3")->second->getName());
	ASSERT_EQ("c_county4", ownedDJ8.find("c_county4")->second->getName());
	ASSERT_EQ("c_county5", ownedDJ8.find("c_county5")->second->getName());
	ASSERT_EQ(3, ownedDF8.size());
	ASSERT_EQ("c_county1", ownedDF8.find("c_county1")->second->getName());
	ASSERT_EQ("c_county2", ownedDF8.find("c_county2")->second->getName());
	ASSERT_EQ("c_county3", ownedDF8.find("c_county3")->second->getName());
}

TEST(CK3World_TitlesTests, titlesCanBeCongregated)
{
	std::stringstream input;
	input << "1 = { key = c_county1 holder = 1 de_facto_liege = 6 de_jure_liege = 6 }\n";
	input << "2 = { key = c_county2 holder = 2 de_facto_liege = 6 de_jure_liege = 6  }\n";
	input << "3 = { key = c_county3 holder = 3 de_facto_liege = 6 de_jure_liege = 7  }\n";
	input << "4 = { key = c_county4 holder = 4 de_facto_liege = 7 de_jure_liege = 7  }\n";
	input << "5 = { key = c_county5 holder = 5 de_facto_liege = 7 de_jure_liege = 7  }\n";
	input << "6 = { key = d_duchy1 holder = 6 de_facto_liege = 8 de_jure_liege = 8 de_jure_vassals = { 1 2 } }\n";
	input << "7 = { key = d_duchy2 holder = 7 de_jure_liege = 8  de_jure_vassals = { 3 4 5 } }\n";
	input << "8 = { key = k_kingdom1 holder = 8 de_jure_vassals = { 6 7 } }\n";
	CK3::Titles titles(input);
	titles.linkTitles();
	titles.getTitles().find("k_kingdom1")->second->congregateDFCounties();
	titles.getTitles().find("k_kingdom1")->second->congregateDJCounties();

	const auto& ownedDF8 = titles.getTitles().find("k_kingdom1")->second->getOwnedDFCounties();
	const auto& ownedDJ8 = titles.getTitles().find("k_kingdom1")->second->getOwnedDJCounties();

	ASSERT_EQ(5, ownedDJ8.size());
	ASSERT_EQ("c_county1", ownedDJ8.find("c_county1")->second->getName());
	ASSERT_EQ("c_county2", ownedDJ8.find("c_county2")->second->getName());
	ASSERT_EQ("c_county3", ownedDJ8.find("c_county3")->second->getName());
	ASSERT_EQ("c_county4", ownedDJ8.find("c_county4")->second->getName());
	ASSERT_EQ("c_county5", ownedDJ8.find("c_county5")->second->getName());
	ASSERT_EQ(3, ownedDF8.size());
	ASSERT_EQ("c_county1", ownedDF8.find("c_county1")->second->getName());
	ASSERT_EQ("c_county2", ownedDF8.find("c_county2")->second->getName());
	ASSERT_EQ("c_county3", ownedDF8.find("c_county3")->second->getName());
}