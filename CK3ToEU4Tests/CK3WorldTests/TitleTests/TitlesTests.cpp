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

TEST(CK3World_TitlesTests, junkTitlesAreIgnored)
{
	std::stringstream input;
	input << "123={key=c_roma}\n";
	input << "1234=none\n";
	input << "12345={key=c_roma2}\n";

	const CK3::Titles titles(input);

	ASSERT_EQ(2, titles.getTitles().size());
}
