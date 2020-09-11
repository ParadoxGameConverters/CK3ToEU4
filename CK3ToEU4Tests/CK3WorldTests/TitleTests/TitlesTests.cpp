#include "../../CK3toEU4/Source/CK3World/Titles/Title.h"
#include "../../CK3toEU4/Source/CK3World/Titles/Titles.h"
#include "../../CK3toEU4/Source/CK3World/CoatsOfArms/CoatsOfArms.h"
#include "../../CK3toEU4/Source/CK3World/CoatsOfArms/CoatOfArms.h"
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
	input << "1 = { key = c_county1 de_facto_liege = 6 de_jure_liege = 6 }\n";
	input << "2 = { key = c_county2 de_facto_liege = 6 de_jure_liege = 6  }\n";
	input << "3 = { key = c_county3 de_facto_liege = 6 de_jure_liege = 7  }\n";
	input << "4 = { key = c_county4 de_facto_liege = 7 de_jure_liege = 7  }\n";
	input << "5 = { key = c_county5 de_facto_liege = 7 de_jure_liege = 7  }\n";
	input << "6 = { key = d_duchy1 de_facto_liege = 8 de_jure_liege = 8 de_jure_vassals = { 1 2 } }\n";
	input << "7 = { key = d_duchy2 de_jure_liege = 8  de_jure_vassals = { 3 4 5 } }\n";
	input << "8 = { key = k_kingdom1 de_jure_vassals = { 6 7 } }\n";
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
	ASSERT_FALSE(t1->second->getDJVassals());
	ASSERT_FALSE(t2->second->getDJVassals());
	ASSERT_FALSE(t3->second->getDJVassals());
	ASSERT_FALSE(t4->second->getDJVassals());
	ASSERT_FALSE(t5->second->getDJVassals());
	ASSERT_EQ(2, t6->second->getDJVassals()->size());
	ASSERT_EQ("c_county1", t6->second->getDJVassals()->find(1)->second->getName());
	ASSERT_EQ("c_county2", t6->second->getDJVassals()->find(2)->second->getName());
	ASSERT_EQ(3, t7->second->getDJVassals()->size());
	ASSERT_EQ("c_county3", t7->second->getDJVassals()->find(3)->second->getName());
	ASSERT_EQ("c_county4", t7->second->getDJVassals()->find(4)->second->getName());
	ASSERT_EQ("c_county5", t7->second->getDJVassals()->find(5)->second->getName());
	ASSERT_EQ(2, t8->second->getDJVassals()->size());
	ASSERT_EQ("d_duchy1", t8->second->getDJVassals()->find(6)->second->getName());
	ASSERT_EQ("d_duchy2", t8->second->getDJVassals()->find(7)->second->getName());

	// Testing defacto vassals
	ASSERT_FALSE(t1->second->getDJVassals());
	ASSERT_FALSE(t2->second->getDJVassals());
	ASSERT_FALSE(t3->second->getDJVassals());
	ASSERT_FALSE(t4->second->getDJVassals());
	ASSERT_FALSE(t5->second->getDJVassals());
	ASSERT_EQ(3, t6->second->getDFVassals()->size());
	ASSERT_EQ("c_county1", t6->second->getDFVassals()->find(1)->second->getName());
	ASSERT_EQ("c_county2", t6->second->getDFVassals()->find(2)->second->getName());
	ASSERT_EQ("c_county3", t6->second->getDFVassals()->find(3)->second->getName());
	ASSERT_EQ(2, t7->second->getDFVassals()->size());
	ASSERT_EQ("c_county4", t7->second->getDFVassals()->find(4)->second->getName());
	ASSERT_EQ("c_county5", t7->second->getDFVassals()->find(5)->second->getName());
	ASSERT_EQ(1, t8->second->getDFVassals()->size());
	ASSERT_EQ("d_duchy1", t8->second->getDFVassals()->find(6)->second->getName());
}
