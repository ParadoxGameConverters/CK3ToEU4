#include "../../CK3toEU4/Source/EU4World/Province/EU4Province.h"
#include "../../CK3toEU4/Source/Mappers/RegionMapper/Area.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_AreaTests, blankAreasLoadForNoProvinces)
{
	std::stringstream input;

	const mappers::Area newArea(input);

	ASSERT_TRUE(newArea.getProvinces().empty());
}

TEST(Mappers_AreaTests, provincesCanBeLoaded)
{
	std::stringstream input;
	input << "{ 1 2 3 }";

	const mappers::Area newArea(input);

	ASSERT_FALSE(newArea.getProvinces().empty());
	ASSERT_EQ(3, newArea.getProvinces().size(), 3);
	ASSERT_EQ(1, newArea.getProvinces().find(1)->first);
	ASSERT_EQ(2, newArea.getProvinces().find(2)->first);
	ASSERT_EQ(3, newArea.getProvinces().find(3)->first);
}

TEST(Mappers_AreaTests, provincesCanBeFound)
{
	std::stringstream input;
	input << "{ 1 2 3 }";

	const mappers::Area newArea(input);

	ASSERT_TRUE(newArea.areaContainsProvince(1));
	ASSERT_TRUE(newArea.areaContainsProvince(2));
	ASSERT_TRUE(newArea.areaContainsProvince(3));
}

TEST(Mappers_AreaTests, provinceMismatchReturnsFalse)
{
	std::stringstream input;
	input << "{ 1 2 3 }";

	const mappers::Area newArea(input);

	ASSERT_FALSE(newArea.areaContainsProvince(4));
}

TEST(Mappers_AreaTests, provinceCanLinkToEU4Province)
{
	std::stringstream input;
	input << "{ 1 2 3 }";
	mappers::Area newArea(input);

	auto eu4Province = std::make_shared<EU4::Province>();

	ASSERT_FALSE(newArea.getProvinces().find(2)->second);
	newArea.linkProvince(std::pair(2, eu4Province));
	ASSERT_TRUE(newArea.getProvinces().find(2)->second);
}
