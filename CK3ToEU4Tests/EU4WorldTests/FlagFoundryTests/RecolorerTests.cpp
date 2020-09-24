#include "../CK3toEU4/Source/EU4World/FlagFoundry/Recolorer.h"
#include "../CK3toEU4/Source/EU4World/FlagFoundry/Warehouse.h"
#include "gtest/gtest.h"
#include "Magick++.h"
#include <sstream>

TEST(EU4World_RecolorerTests, noColorGraysSolidPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.imageData = Magick::Image("TestFiles/FlagFoundry/pattern_solid.dds");
	auto result = EU4::Recolorer::craftPatternImage(imageBlock);
	result.write("result.dds"); // Testing versus any amount of gray requires to write it down first. Unknown why. Possibly their bug.

	const Magick::Image test("result.dds");
	const Magick::Image orig("TestFiles/FlagFoundry/solid_gray.dds");

	ASSERT_TRUE(orig.compare(test));							 // True return when 2 pictures are same.
	ASSERT_FALSE(result.compare(imageBlock.imageData)); // False because it was changed
}

TEST(EU4World_RecolorerTests, color1RecolorsMask1SolidPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color1 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.imageData = Magick::Image("TestFiles/FlagFoundry/pattern_solid.dds");	
	const auto result = EU4::Recolorer::craftPatternImage(imageBlock);

	const Magick::Image test("TestFiles/FlagFoundry/solid_cyan.dds");

	ASSERT_TRUE(result.compare(test));
}

TEST(EU4World_RecolorerTests, color2GraysMask1SolidPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color2 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.imageData = Magick::Image("TestFiles/FlagFoundry/pattern_solid.dds");
	auto result = EU4::Recolorer::craftPatternImage(imageBlock);
	result.write("result.dds");

	const Magick::Image test("result.dds");
	const Magick::Image orig("TestFiles/FlagFoundry/solid_gray.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color3GraysMask1SolidPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color3 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.imageData = Magick::Image("TestFiles/FlagFoundry/pattern_solid.dds");
	auto result = EU4::Recolorer::craftPatternImage(imageBlock);
	result.write("result.dds");
	
	const Magick::Image test("result.dds");
	const Magick::Image orig("TestFiles/FlagFoundry/solid_gray.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, colorSuiteRecolorsSolidPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color1 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.color2 = commonItems::Color(std::array<int, 3>{255, 255, 0}); // recoloring to yellow.
	imageBlock.color3 = commonItems::Color(std::array<int, 3>{255, 0, 255}); // recoloring to magenta.
	imageBlock.imageData = Magick::Image("TestFiles/FlagFoundry/pattern_solid.dds");
	const auto result = EU4::Recolorer::craftPatternImage(imageBlock);

	const Magick::Image test("TestFiles/FlagFoundry/solid_cyan.dds");

	ASSERT_TRUE(result.compare(test));
}

TEST(EU4World_RecolorerTests, color1RecolorsHalfOfTwocolorPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color1 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.imageData = Magick::Image("TestFiles/FlagFoundry/pattern_shield.dds");
	auto result = EU4::Recolorer::craftPatternImage(imageBlock);
	result.write("result.dds");

	const Magick::Image test("result.dds");
	const Magick::Image orig("TestFiles/FlagFoundry/shield_half.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color2RecolorsOtherHalfOfTwocolorPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color2 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.imageData = Magick::Image("TestFiles/FlagFoundry/pattern_shield.dds");
	auto result = EU4::Recolorer::craftPatternImage(imageBlock);
	result.write("result.dds");
	
	const Magick::Image test("result.dds");
	const Magick::Image orig("TestFiles/FlagFoundry/shield_half2.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color3GraysTwocolorPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color3 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.imageData = Magick::Image("TestFiles/FlagFoundry/pattern_shield.dds");
	auto result = EU4::Recolorer::craftPatternImage(imageBlock);
	result.write("result.dds");

	const Magick::Image test("result.dds");
	const Magick::Image orig("TestFiles/FlagFoundry/solid_gray.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, colorSuiteRecolorsTwoColorPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color1 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.color2 = commonItems::Color(std::array<int, 3>{0, 255, 0}); // recoloring to green.
	imageBlock.color3 = commonItems::Color(std::array<int, 3>{255, 0, 255}); // recoloring to magenta.
	imageBlock.imageData = Magick::Image("TestFiles/FlagFoundry/pattern_shield.dds");
	auto result = EU4::Recolorer::craftPatternImage(imageBlock);
	result.write("result.dds");

	const Magick::Image test("result.dds");
	const Magick::Image orig("TestFiles/FlagFoundry/shield_full.dds");

	ASSERT_TRUE(orig.compare(test));
}
