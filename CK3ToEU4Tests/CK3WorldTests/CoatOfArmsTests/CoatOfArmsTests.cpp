#include "../CK3ToEU4/Source/CK3World/CoatsOfArms/CoatOfArms.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_CoatOfArmsTests, IDCanBeSet)
{
	std::stringstream input;
	const CK3::CoatOfArms coatOfArms(input, 79);

	ASSERT_EQ(79, coatOfArms.getID());
}


TEST(CK3World_CoatOfArmsTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::CoatOfArms coatOfArms(input, 79);

	ASSERT_FALSE(coatOfArms.getPattern());
	ASSERT_FALSE(coatOfArms.getColor1());
	ASSERT_FALSE(coatOfArms.getColor2());
	ASSERT_FALSE(coatOfArms.getColor3());
	ASSERT_TRUE(coatOfArms.getTexturedEmblems().empty());
	ASSERT_TRUE(coatOfArms.getColoredEmblems().empty());
	ASSERT_TRUE(coatOfArms.getSubs().empty());
	ASSERT_TRUE(coatOfArms.getInstances().empty());
	ASSERT_FALSE(coatOfArms.getParent());
}

TEST(CK3World_CoatOfArmsTests, PrimitivesCanBeLoaded)
{
	laFabricaDeColor.clear();
	laFabricaDeColor.addNamedColor("white", commonItems::Color(std::array<int, 3>{255, 255, 255}));
	laFabricaDeColor.addNamedColor("blue", commonItems::Color(std::array<int, 3>{0, 0, 255}));
	laFabricaDeColor.addNamedColor("red", commonItems::Color(std::array<int, 3>{255, 0, 0}));
	laFabricaDeColor.addNamedColor("green", commonItems::Color(std::array<int, 3>{0, 255, 0}));
	laFabricaDeColor.addNamedColor("gray", commonItems::Color(std::array<int, 3>{127, 127, 127}));

	std::stringstream input;
	input << "pattern = \"infinite\"\n";
	input << "color1 = white\n";
	input << "color2 = \"blue\"\n";
	input << "color3 = red\n";
	input << "parent = \"k_abyss\"\n";

	const CK3::CoatOfArms coatOfArms(input, 79);

	ASSERT_EQ("infinite", coatOfArms.getPattern());
	ASSERT_EQ("= hex { ffffff }", coatOfArms.getColor1()->outputHex());
	ASSERT_EQ("= hex { 0000ff }", coatOfArms.getColor2()->outputHex());
	ASSERT_EQ("= hex { ff0000 }", coatOfArms.getColor3()->outputHex());
	ASSERT_EQ("k_abyss", coatOfArms.getParent()->first);

	laFabricaDeColor.clear();
}

TEST(CK3World_CoatOfArmsTests, EmblemsCanBeLoaded)
{
	std::stringstream input;
	input << "textured_emblem = {\n";
	input << "texture = \"rough\"\n";
	input << "}\n";
	input << "textured_emblem = {\n";
	input << "texture = \"bumpy\"\n";
	input << "}\n";
	input << "colored_emblem = {\n";
	input << "texture = \"smooth\"\n";
	input << "}\n";
	input << "colored_emblem = {\n";
	input << "texture = \"lumpy\"\n";
	input << "}\n";
	input << "colored_emblem = {\n";
	input << "texture = \"silky\"\n";
	input << "}\n";

	const CK3::CoatOfArms coatOfArms(input, 79);

	ASSERT_EQ(2, coatOfArms.getTexturedEmblems().size());
	ASSERT_EQ("rough", coatOfArms.getTexturedEmblems()[0].getTexture());
	ASSERT_EQ("bumpy", coatOfArms.getTexturedEmblems()[1].getTexture());
	ASSERT_EQ(3, coatOfArms.getColoredEmblems().size());
	ASSERT_EQ("smooth", coatOfArms.getColoredEmblems()[0].getTexture());
	ASSERT_EQ("lumpy", coatOfArms.getColoredEmblems()[1].getTexture());
	ASSERT_EQ("silky", coatOfArms.getColoredEmblems()[2].getTexture());
}

TEST(CK3World_CoatOfArmsTests, SubcoatsCanBeLoaded)
{
	std::stringstream input;
	input << "sub = {\n";
	input << "pattern = \"funny\"\n";
	input << "}\n";
	input << "sub = {\n";
	input << "pattern = \"sad\"\n";
	input << "}\n";

	const CK3::CoatOfArms coatOfArms(input, 79);

	ASSERT_EQ(2, coatOfArms.getSubs().size());
	ASSERT_EQ("funny", coatOfArms.getSubs()[0]->getPattern());
	ASSERT_EQ("sad", coatOfArms.getSubs()[1]->getPattern());
}

TEST(CK3World_CoatOfArmsTests, InstancesCanBeLoaded)
{
	std::stringstream input;
	input << "instance = {\n";
	input << "depth = 3.17\n";
	input << "}\n";
	input << "instance = {\n";
	input << "depth = -8.999\n";
	input << "}\n";

	const CK3::CoatOfArms coatOfArms(input, 79);

	ASSERT_EQ(2, coatOfArms.getInstances().size());
	ASSERT_NEAR(3.17, coatOfArms.getInstances()[0].getDepth(), 0.001);
	ASSERT_NEAR(-8.999, coatOfArms.getInstances()[1].getDepth(), 0.001);
}
