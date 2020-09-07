#include "../CK3toEU4/Source/CK3World/CoatsOfArms/EmblemInstance.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_EmblemInstanceTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::EmblemInstance emblemInstance(input);

	ASSERT_EQ(0.0f, emblemInstance.getRotation());
	ASSERT_EQ(0.0f, emblemInstance.getDepth());
	ASSERT_TRUE(emblemInstance.getPosition().empty());
	ASSERT_TRUE(emblemInstance.getScale().empty());
	ASSERT_TRUE(emblemInstance.getOffset().empty());
}

TEST(CK3World_EmblemInstanceTests, emblemInstancePrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "rotation = -0.41\n";
	input << "depth = 3.17\n";
	input << "position = { 0.13 0.34 }\n";
	input << "scale = { -0.23 0.44 }\n";
	input << "offset = { 0.33 -0.54 }\n";

	const CK3::EmblemInstance emblemInstance(input);

	ASSERT_NEAR(-0.41, emblemInstance.getRotation(), 0.001);
	ASSERT_NEAR(3.17, emblemInstance.getDepth(), 0.001);
	ASSERT_EQ(2, emblemInstance.getPosition().size());
	ASSERT_NEAR(0.13, emblemInstance.getPosition()[0], 0.001);
	ASSERT_NEAR(0.34, emblemInstance.getPosition()[1], 0.001);
	ASSERT_EQ(2, emblemInstance.getScale().size());
	ASSERT_NEAR(-0.23, emblemInstance.getScale()[0], 0.001);
	ASSERT_NEAR(0.44, emblemInstance.getScale()[1], 0.001);
	ASSERT_EQ(2, emblemInstance.getOffset().size());
	ASSERT_NEAR(0.33, emblemInstance.getOffset()[0], 0.001);
	ASSERT_NEAR(-0.54, emblemInstance.getOffset()[1], 0.001);
}
