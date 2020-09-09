#include "../../CK3toEU4/Source/CK3World/Geography/CountyDetail.h"
#include "../../CK3toEU4/Source/CK3World/Geography/CountyDetails.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_CountyDetailsTests, detailsDefaultToEmpty)
{
	std::stringstream input;
	const CK3::CountyDetails details(input);

	ASSERT_TRUE(details.getCountyDetails().empty());
}

TEST(CK3World_CountyDetailsTests, detailsCanBeLoadedIfPresent)
{
	std::stringstream input;
	input << "c_county1 = {}\n";
	input << "c_county2 = { development = 8 }\n";
	input << "c_county3 = { faith = 4 }\n";

	const CK3::CountyDetails details(input);
	const auto& c1 = details.getCountyDetails().find("c_county1");
	const auto& c2 = details.getCountyDetails().find("c_county2");
	const auto& c3 = details.getCountyDetails().find("c_county3");

	ASSERT_EQ(3, details.getCountyDetails().size());
	ASSERT_FALSE(c1->second->getDevelopment());
	ASSERT_EQ(8, c2->second->getDevelopment());
	ASSERT_EQ(4, c3->second->getFaith().first);
}
