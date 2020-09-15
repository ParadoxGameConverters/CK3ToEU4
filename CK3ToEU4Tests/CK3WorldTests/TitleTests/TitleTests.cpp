#include "../CK3toEU4/Source/CK3World/Titles/Title.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_TitleTests, idCanBeSet)
{
	std::stringstream input;
	const CK3::Title theTitle(input, 1);

	ASSERT_EQ(1, theTitle.getID());
}

TEST(CK3World_TitleTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::Title theTitle(input, 1);

	ASSERT_TRUE(theTitle.getName().empty());
	ASSERT_TRUE(theTitle.getDisplayName().empty());
	ASSERT_TRUE(theTitle.getAdjective().empty());
	ASSERT_TRUE(theTitle.getHistoryGovernment().empty());
	ASSERT_TRUE(theTitle.getLaws().empty());
	ASSERT_FALSE(theTitle.getHolder());
	ASSERT_FALSE(theTitle.isTheocraticLease());
	ASSERT_FALSE(theTitle.isCountyCapitalBarony());
	ASSERT_FALSE(theTitle.isDuchyCapitalBarony());
	ASSERT_FALSE(theTitle.getCapital().first);
	ASSERT_FALSE(theTitle.getDFLiege());
	ASSERT_FALSE(theTitle.getDJLiege());
	ASSERT_TRUE(theTitle.getDJVassals().empty());
	ASSERT_TRUE(theTitle.getHeirs().empty());
	ASSERT_TRUE(theTitle.getClaimants().empty());
	ASSERT_TRUE(theTitle.getElectors().empty());
	ASSERT_FALSE(theTitle.getCoA());
	ASSERT_FALSE(theTitle.getColor());
	ASSERT_FALSE(theTitle.isLandless());
}

TEST(CK3World_TitleTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "key=\"k_grenada\"\n";
	input << "name=\"Cuman Empire\"\n";
	input << "adj=\"the \"\n";
	input << "history_government=\"theocratic_government\"\n";
	input << "laws={ law_1 second_law second_law second_law law_succession_3 }\n";
	input << "holder=123\n";
	input << "theocratic_lease=yes\n";
	input << "capital_barony=yes\n";
	input << "duchy_capital_barony=yes\n";
	input << "capital=123\n";
	input << "de_facto_liege=1234\n";
	input << "de_jure_liege=12345\n";
	input << "de_jure_vassals={ 1 2 3 4 5 }\n";
	input << "heir={ 3 4 5 }\n";
	input << "claim={ 7 8 }\n";
	input << "coat_of_arms_id=45\n";
	input << "color={ 1 2 3 }\n";
	input << "landless=yes\n";

	const CK3::Title theTitle(input, 1);

	ASSERT_EQ("k_grenada", theTitle.getName());
	ASSERT_EQ("Cuman Empire", theTitle.getDisplayName());
	ASSERT_EQ("the ", theTitle.getAdjective());
	ASSERT_EQ("theocratic_government", theTitle.getHistoryGovernment());
	ASSERT_EQ(3, theTitle.getLaws().size());
	ASSERT_EQ(1, theTitle.getLaws().count("second_law"));
	ASSERT_EQ(123, theTitle.getHolder()->first);
	ASSERT_TRUE(theTitle.isTheocraticLease());
	ASSERT_TRUE(theTitle.isCountyCapitalBarony());
	ASSERT_TRUE(theTitle.isDuchyCapitalBarony());
	ASSERT_EQ(123, theTitle.getCapital().first);
	ASSERT_EQ(1234, theTitle.getDFLiege()->first);
	ASSERT_EQ(12345, theTitle.getDJLiege()->first);
	ASSERT_EQ(5, theTitle.getDJVassals().size());
	ASSERT_EQ(1, theTitle.getDJVassals().count(1));
	ASSERT_EQ(3, theTitle.getHeirs().size());
	ASSERT_EQ(3, theTitle.getHeirs()[0].first);
	ASSERT_EQ(4, theTitle.getHeirs()[1].first);
	ASSERT_EQ(5, theTitle.getHeirs()[2].first);
	ASSERT_EQ(2, theTitle.getClaimants().size());
	ASSERT_EQ(1, theTitle.getClaimants().count(7));
	ASSERT_EQ(1, theTitle.getClaimants().count(8));
	ASSERT_EQ(45, theTitle.getCoA()->first);
	ASSERT_EQ(commonItems::Color(std::array<int, 3>{1, 2, 3}), theTitle.getColor());
	ASSERT_TRUE(theTitle.isLandless());
}

TEST(CK3World_TitleTests, successionElectionCanBeLoaded)
{
	std::stringstream input;
	input << "succession_election={\n";
	input << "\telectors={ 17972 43082 16968 }\n";
	input << "}\n";

	const CK3::Title theTitle(input, 1);

	ASSERT_EQ(3, theTitle.getElectors().size());
	ASSERT_EQ(17972, theTitle.getElectors().find(17972)->first);
	ASSERT_EQ(43082, theTitle.getElectors().find(43082)->first);
	ASSERT_EQ(16968, theTitle.getElectors().find(16968)->first);
}
