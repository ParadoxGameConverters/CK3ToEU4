#include "../CK3toEU4/Source/Mappers/ProvinceMapper/ProvinceMapping.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_ProvinceMappingTests, EU4ProvinceCanBeAdded)
{
	std::stringstream input;
	input << "= { eu4 = 2 eu4 = 1 }";

	const mappers::ProvinceMapping theMapper(input);

	ASSERT_EQ(theMapper.getEU4Provinces()[0], 2);
	ASSERT_EQ(theMapper.getEU4Provinces()[1], 1);
}

TEST(Mappers_ProvinceMappingTests, CK3ProvinceCanBeAdded)
{
	std::stringstream input;
	input << "= { ck3 = 2 ck3 = 1 }";

	const mappers::ProvinceMapping theMapper(input);

	ASSERT_EQ(theMapper.getCK3Provinces()[0], 2);
	ASSERT_EQ(theMapper.getCK3Provinces()[1], 1);
}
