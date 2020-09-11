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
