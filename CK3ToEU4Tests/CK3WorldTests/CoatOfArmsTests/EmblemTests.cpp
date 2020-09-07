#include "../CK3toEU4/Source/CK3World/CoatsOfArms/Emblem.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_EmblemTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::Emblem emblem(input);

	ASSERT_TRUE(emblem.getTexture().empty());
	ASSERT_FALSE(emblem.getColor1());
	ASSERT_FALSE(emblem.getColor2());
	ASSERT_FALSE(emblem.getColor3());
	ASSERT_TRUE(emblem.getMask().empty());
	ASSERT_TRUE(emblem.getInstances().empty());
}

TEST(CK3World_EmblemTests, emblemPrimitivesCanBeLoaded)
{
	laFabricaDeColor.clear();
	laFabricaDeColor.addNamedColor("white", commonItems::Color(std::array<int, 3>{255, 255, 255}));
	laFabricaDeColor.addNamedColor("blue", commonItems::Color(std::array<int, 3>{0, 0, 255}));
	laFabricaDeColor.addNamedColor("red", commonItems::Color(std::array<int, 3>{255, 0, 0}));
	
	std::stringstream input;
	input << "texture = \"so_smooth\"\n";
	input << "color1 = white\n";
	input << "color2 = \"blue\"\n";
	input << "color3 = red\n";
	input << "mask = { 2 3 }\n";

	const CK3::Emblem emblem(input);

	ASSERT_EQ("so_smooth", emblem.getTexture());
	ASSERT_EQ("= hex { ffffff }", emblem.getColor1()->outputHex());
	ASSERT_EQ("= hex { 0000ff }", emblem.getColor2()->outputHex());
	ASSERT_EQ("= hex { ff0000 }", emblem.getColor3()->outputHex());
	ASSERT_EQ(2, emblem.getMask().size());
	ASSERT_EQ(2, emblem.getMask()[0]);
	ASSERT_EQ(3, emblem.getMask()[1]);

	laFabricaDeColor.clear();
}
