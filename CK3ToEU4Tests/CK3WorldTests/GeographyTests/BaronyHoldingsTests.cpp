#include "../../CK3toEU4/Source/CK3World/Geography/BaronyHolding.h"
#include "../../CK3toEU4/Source/CK3World/Geography/BaronyHoldings.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_BaronyHoldingsTests, baronyHoldingsDefaultToEmpty)
{
	std::stringstream input;
	const CK3::BaronyHoldings baronies(input);

	ASSERT_TRUE(baronies.getBaronyHoldings().empty());
}

TEST(CK3World_BaronyHoldingsTests, baronyHoldingsCanBeLoadedIfPresent)
{
	std::stringstream input;
	input << "11={}\n";
	input << "13={holding={}}\n";
	input << "15={holding={type=\"castle_holding\"\nbuildings={ {type=\"hunting_grounds_01\"} {} {} {type=\"hill_farms_02\"} {} {}}}\n";

	const CK3::BaronyHoldings baronies(input);
	const auto& b1 = baronies.getBaronyHoldings().find(11);
	const auto& b2 = baronies.getBaronyHoldings().find(13);
	const auto& b3 = baronies.getBaronyHoldings().find(15);

	ASSERT_TRUE(b1->second->getHoldingType().empty());
	ASSERT_TRUE(b2->second->getHoldingType().empty());
	ASSERT_EQ("castle_holding", b3->second->getHoldingType());
	ASSERT_EQ(2, b3->second->getBuildings().size());
}
