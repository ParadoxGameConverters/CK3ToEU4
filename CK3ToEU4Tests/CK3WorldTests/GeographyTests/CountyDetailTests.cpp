#include "../CK3ToEU4/Source/CK3World/Geography/CountyDetail.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_CountyDetailTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::CountyDetail detail(input);

	ASSERT_FALSE(detail.getDevelopment());
	ASSERT_FALSE(detail.getCulture().first);
	ASSERT_FALSE(detail.getFaith().first);
}

TEST(CK3World_CountyDetailTests, detailPrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "development = 7\n";
	input << "culture = 9\n";
	input << "faith = 5\n";

	const CK3::CountyDetail detail(input);

	ASSERT_EQ(7, detail.getDevelopment());
	ASSERT_EQ(9, detail.getCulture().first);
	ASSERT_EQ(5, detail.getFaith().first);
}
