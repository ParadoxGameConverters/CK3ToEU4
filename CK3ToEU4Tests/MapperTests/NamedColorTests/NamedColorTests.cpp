#include "../../CK3toEU4/Source/Mappers/NamedColors/NamedColors.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_NamedColorsTests, NoInputLoadsNoColors)
{		
	std::stringstream input;
	mappers::NamedColors colors;
	colors.loadColors(input);

	EXPECT_TRUE(laFabricaDeColor.getRegisteredColors().empty());
}

TEST(Mappers_NamedColorsTests, ColorsCanBeLoaded)
{
	std::stringstream input;
	input << "red 		= hsv {		0.02 	0.8 	0.45 	}\n";
	input << "blue 		= hsv {		0.58 	0.8 	0.4 	} \n";
	input << "brown 		= hsv360 {	021 	074 	045		} \n";
	
	mappers::NamedColors colors;
	colors.loadColors(input);

	const auto& red = laFabricaDeColor.getColor("red");
	const auto& blue = laFabricaDeColor.getColor("blue");
	const auto& brown = laFabricaDeColor.getColor("brown");

	EXPECT_EQ(3, laFabricaDeColor.getRegisteredColors().size());
	EXPECT_EQ("= hsv { 0.02 0.8 0.45 }", red.outputHsv());
	EXPECT_EQ("= hsv { 0.58 0.8 0.4 }", blue.outputHsv());
	EXPECT_EQ("= hsv360 { 21 74 45 }", brown.outputHsv360());
}

TEST(Mappers_NamedColorsTests, ColorsCanBeOverriddenByMods)
{
	laFabricaDeColor.clear();
	mappers::NamedColors colors;

	std::stringstream input;
	input << "red 		= hsv {		0.02 	0.8 	0.45 	}\n";
	input << "gold 		= hsv {		0.58 	0.8 	0.4 	} \n";
	colors.loadColors(input);
	
	std::stringstream input2;
	input2 << "gold 		= hsv360 {	021 	074 	045		} \n";
	colors.loadColors(input2);

	const auto& red = laFabricaDeColor.getColor("red");
	const auto& gold = laFabricaDeColor.getColor("gold");

	EXPECT_EQ(2, laFabricaDeColor.getRegisteredColors().size());
	EXPECT_EQ("= hsv { 0.02 0.8 0.45 }", red.outputHsv());
	EXPECT_EQ("= hsv360 { 21 74 45 }", gold.outputHsv360());
}
