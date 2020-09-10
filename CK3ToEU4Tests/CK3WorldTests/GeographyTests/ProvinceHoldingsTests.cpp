#include "../../CK3toEU4/Source/CK3World/Geography/ProvinceHolding.h"
#include "../../CK3toEU4/Source/CK3World/Geography/ProvinceHoldings.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_ProvinceHoldingsTests, ProvinceHoldingsDefaultToEmpty)
{
	std::stringstream input;
	const CK3::ProvinceHoldings baronies(input);

	ASSERT_TRUE(baronies.getProvinceHoldings().empty());
}

TEST(CK3World_ProvinceHoldingsTests, ProvinceHoldingsCanBeLoadedIfPresent)
{
	std::stringstream input;
	input << "11={}\n";
	input << "13={holding={}}\n";
	input << "15={holding={type=\"castle_holding\"\nbuildings={ {type=\"hunting_grounds_01\"} {} {} {type=\"hill_farms_02\"} {} {}}}\n";

	const CK3::ProvinceHoldings baronies(input);
	const auto& b1 = baronies.getProvinceHoldings().find(11);
	const auto& b2 = baronies.getProvinceHoldings().find(13);
	const auto& b3 = baronies.getProvinceHoldings().find(15);

	ASSERT_TRUE(b1->second->getHoldingType().empty());
	ASSERT_TRUE(b2->second->getHoldingType().empty());
	ASSERT_EQ("castle_holding", b3->second->getHoldingType());
	ASSERT_EQ(2, b3->second->getBuildings().size());
}
