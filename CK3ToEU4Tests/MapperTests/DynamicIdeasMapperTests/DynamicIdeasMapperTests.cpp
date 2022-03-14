#include "../CK3ToEU4/Source/Mappers/DynamicIdeasMapper/DynamicIdeasMapper.h"
#include "gtest/gtest.h"
#include <sstream>

// TEST(Mappers_DynamicIdeasMapperTests, brokenLinkIsIgnored)

TEST(Mappers_DynamicIdeasMapperTests, linksMakeMaps)
{
	std::stringstream input;
	input << "link = { ck3 = { ethos = ethos_bellicose } eu4 = { yearly_army_professionalism = 0.01 land_forcelimit = 4 } }";
	input << "link = { ck3 = { tradition = tradition_african_tolerance } eu4 = { religious_unity = 0.5 } } ";
	input << "link = { ck3 = { default = increase_discipline } eu4 = { discipline = 0.025 } }";

	const mappers::DynamicIdeasMapper theMapper(input);

	ASSERT_EQ(theMapper.getEthosMap().size(), 1);
	ASSERT_EQ(theMapper.getTraditionMap().size(), 2);
	ASSERT_EQ(theMapper.getDefaults().size(), 1);
	ASSERT_TRUE(theMapper.getEthosMap().contains("ethos_bellicose"));
	ASSERT_TRUE(theMapper.getTraditionMap().contains("tradition_african_tolerance"));
	ASSERT_TRUE(theMapper.getTraditionMap().contains("increase_discipline"));
	ASSERT_TRUE(theMapper.getDefaults().front().compare("increase_discipline") == 0);
}