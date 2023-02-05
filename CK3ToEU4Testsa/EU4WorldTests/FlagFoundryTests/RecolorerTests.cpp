#include "../CK3ToEU4/Source/EU4World/FlagFoundry/Recolorer.h"
#include "../CK3ToEU4/Source/EU4World/FlagFoundry/Warehouse.h"
#include "Magick++.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(EU4World_RecolorerTests, noColorGraysSolidPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.imageData = Magick::Image("FlagFoundry/patterns/pattern_solid.dds");
	auto recolorerTest = EU4::Recolorer::craftPatternImage(imageBlock);
	recolorerTest.write("recolorerTest1.dds"); // Testing versus any amount of gray requires to write it down first. Unknown why. Possibly their bug.

	const Magick::Image test("recolorerTest1.dds");
	const Magick::Image orig("FlagFoundry/p_solid_gray.dds");

	ASSERT_TRUE(orig.compare(test));									  // True return when 2 pictures are same.
	ASSERT_FALSE(recolorerTest.compare(imageBlock.imageData)); // False because it was changed
}

TEST(EU4World_RecolorerTests, color1RecolorsMask1SolidPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color1 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.imageData = Magick::Image("FlagFoundry/patterns/pattern_solid.dds");
	const auto recolorerTest = EU4::Recolorer::craftPatternImage(imageBlock);

	const Magick::Image test("FlagFoundry/p_solid_cyan.dds");

	ASSERT_TRUE(recolorerTest.compare(test));
}

TEST(EU4World_RecolorerTests, color2GraysMask1SolidPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color2 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.imageData = Magick::Image("FlagFoundry/patterns/pattern_solid.dds");
	auto recolorerTest = EU4::Recolorer::craftPatternImage(imageBlock);
	recolorerTest.write("recolorerTest2.dds");

	const Magick::Image test("recolorerTest2.dds");
	const Magick::Image orig("FlagFoundry/p_solid_gray.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color3GraysMask1SolidPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color3 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.imageData = Magick::Image("FlagFoundry/patterns/pattern_solid.dds");
	auto recolorerTest = EU4::Recolorer::craftPatternImage(imageBlock);
	recolorerTest.write("recolorerTest3.dds");

	const Magick::Image test("recolorerTest3.dds");
	const Magick::Image orig("FlagFoundry/p_solid_gray.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, colorSuiteRecolorsSolidPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color1 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.color2 = commonItems::Color(std::array<int, 3>{255, 255, 0}); // recoloring to yellow.
	imageBlock.color3 = commonItems::Color(std::array<int, 3>{255, 0, 255}); // recoloring to magenta.
	imageBlock.imageData = Magick::Image("FlagFoundry/patterns/pattern_solid.dds");
	const auto recolorerTest = EU4::Recolorer::craftPatternImage(imageBlock);

	const Magick::Image test("FlagFoundry/p_solid_cyan.dds");

	ASSERT_TRUE(recolorerTest.compare(test));
}

TEST(EU4World_RecolorerTests, color1RecolorsHalfOfTwocolorPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color1 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.imageData = Magick::Image("FlagFoundry/patterns/pattern_twocolor.dds");
	auto recolorerTest = EU4::Recolorer::craftPatternImage(imageBlock);
	recolorerTest.write("recolorerTest4.dds");

	const Magick::Image test("recolorerTest4.dds");
	const Magick::Image orig("FlagFoundry/p_twocolor_half.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color2RecolorsOtherHalfOfTwocolorPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color2 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.imageData = Magick::Image("FlagFoundry/patterns/pattern_twocolor.dds");
	auto recolorerTest = EU4::Recolorer::craftPatternImage(imageBlock);
	recolorerTest.write("recolorerTest5.dds");

	const Magick::Image test("recolorerTest5.dds");
	const Magick::Image orig("FlagFoundry/p_twocolor_half2.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color3GraysTwocolorPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color3 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.imageData = Magick::Image("FlagFoundry/patterns/pattern_twocolor.dds");
	auto recolorerTest = EU4::Recolorer::craftPatternImage(imageBlock);
	recolorerTest.write("recolorerTest6.dds");

	const Magick::Image test("recolorerTest6.dds");
	const Magick::Image orig("FlagFoundry/p_solid_gray.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, colorSuiteRecolorsTwocolorPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color1 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.color2 = commonItems::Color(std::array<int, 3>{0, 255, 0});	 // recoloring to green.
	imageBlock.color3 = commonItems::Color(std::array<int, 3>{255, 0, 255}); // recoloring to magenta.
	imageBlock.imageData = Magick::Image("FlagFoundry/patterns/pattern_twocolor.dds");
	auto recolorerTest = EU4::Recolorer::craftPatternImage(imageBlock);
	recolorerTest.write("recolorerTest7.dds");

	const Magick::Image test("recolorerTest7.dds");
	const Magick::Image orig("FlagFoundry/p_twocolor_full.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color1RecolorsThirdOfTricolorPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color1 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.imageData = Magick::Image("FlagFoundry/patterns/pattern_tricolor.dds");
	auto recolorerTest = EU4::Recolorer::craftPatternImage(imageBlock);
	recolorerTest.write("recolorerTest8.dds");

	const Magick::Image test("recolorerTest8.dds");
	const Magick::Image orig("FlagFoundry/p_tricolor_third.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color2RecolorsSecondThirdOfTriColorPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color2 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.imageData = Magick::Image("FlagFoundry/patterns/pattern_tricolor.dds");
	auto recolorerTest = EU4::Recolorer::craftPatternImage(imageBlock);
	recolorerTest.write("recolorerTest9.dds");

	const Magick::Image test("recolorerTest9.dds");
	const Magick::Image orig("FlagFoundry/p_tricolor_third2.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color3RecolorsFinalThirdOfTriColorPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color3 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.imageData = Magick::Image("FlagFoundry/patterns/pattern_tricolor.dds");
	auto recolorerTest = EU4::Recolorer::craftPatternImage(imageBlock);
	recolorerTest.write("recolorerTest10.dds");

	const Magick::Image test("recolorerTest10.dds");
	const Magick::Image orig("FlagFoundry/p_tricolor_third3.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, colorSuiteRecolorsTricolorPattern)
{
	EU4::coloredImage imageBlock;
	imageBlock.color1 = commonItems::Color(std::array<int, 3>{0, 255, 255}); // recoloring to cyan.
	imageBlock.color2 = commonItems::Color(std::array<int, 3>{0, 255, 0});	 // recoloring to green.
	imageBlock.color3 = commonItems::Color(std::array<int, 3>{255, 0, 255}); // recoloring to magenta.
	imageBlock.imageData = Magick::Image("FlagFoundry/patterns/pattern_tricolor.dds");
	auto recolorerTest = EU4::Recolorer::craftPatternImage(imageBlock);
	recolorerTest.write("recolorerTest11.dds");

	const Magick::Image test("recolorerTest11.dds");
	const Magick::Image orig("FlagFoundry/p_tricolor_full.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, noColorGraysSolidTexture)
{
	EU4::coloredImage imageBlock;
	imageBlock.imageData = Magick::Image("FlagFoundry/colored_emblems/texture_solid.dds");
	auto recolorerTest = EU4::Recolorer::craftTextureImage(imageBlock);
	recolorerTest.write("recolorerTest12.dds");

	const Magick::Image test("recolorerTest12.dds");
	const Magick::Image orig("FlagFoundry/t_solid_gray.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color1RecolorsMask1SolidTexture)
{
	EU4::coloredImage imageBlock;
	imageBlock.color1 = commonItems::Color(std::array<int, 3>{255, 215, 0});
	imageBlock.imageData = Magick::Image("FlagFoundry/colored_emblems/texture_solid.dds");
	auto recolorerTest = EU4::Recolorer::craftTextureImage(imageBlock);
	recolorerTest.write("recolorerTest13.dds");

	const Magick::Image test("recolorerTest13.dds");
	const Magick::Image orig("FlagFoundry/t_solid_gold.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color2GraysMask1SolidTexture)
{
	EU4::coloredImage imageBlock;
	imageBlock.color2 = commonItems::Color(std::array<int, 3>{255, 215, 0});
	imageBlock.imageData = Magick::Image("FlagFoundry/colored_emblems/texture_solid.dds");
	auto recolorerTest = EU4::Recolorer::craftTextureImage(imageBlock);
	recolorerTest.write("recolorerTest14.dds");

	const Magick::Image test("recolorerTest14.dds");
	const Magick::Image orig("FlagFoundry/t_solid_gray2.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color3GraysMask1SolidTexture)
{
	EU4::coloredImage imageBlock;
	imageBlock.color3 = commonItems::Color(std::array<int, 3>{255, 215, 0});
	imageBlock.imageData = Magick::Image("FlagFoundry/colored_emblems/texture_solid.dds");
	auto recolorerTest = EU4::Recolorer::craftTextureImage(imageBlock);
	recolorerTest.write("recolorerTest15.dds");

	const Magick::Image test("recolorerTest15.dds");
	const Magick::Image orig("FlagFoundry/t_solid_gray2.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, colorSuiteRecolorsSolidTexture)
{
	EU4::coloredImage imageBlock;
	imageBlock.color1 = commonItems::Color(std::array<int, 3>{255, 215, 0});
	imageBlock.color2 = commonItems::Color(std::array<int, 3>{255, 165, 0});
	imageBlock.color3 = commonItems::Color(std::array<int, 3>{50, 180, 200});
	imageBlock.imageData = Magick::Image("FlagFoundry/colored_emblems/texture_solid.dds");
	auto recolorerTest = EU4::Recolorer::craftTextureImage(imageBlock);
	recolorerTest.write("recolorerTest16.dds");

	const Magick::Image test("recolorerTest16.dds");
	const Magick::Image orig("FlagFoundry/t_solid_gold.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color1RecolorsHalfOfTwocoloTexture)
{
	EU4::coloredImage imageBlock;
	imageBlock.color1 = commonItems::Color(std::array<int, 3>{255, 215, 0});
	imageBlock.imageData = Magick::Image("FlagFoundry/colored_emblems/texture_twocolor.dds");
	auto recolorerTest = EU4::Recolorer::craftTextureImage(imageBlock);
	recolorerTest.write("recolorerTest17.dds");

	const Magick::Image test("recolorerTest17.dds");
	const Magick::Image orig("FlagFoundry/t_twocolor_half.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color2RecolorsOtherHalfOfTwocolorTexture)
{
	EU4::coloredImage imageBlock;
	imageBlock.color2 = commonItems::Color(std::array<int, 3>{255, 215, 0});
	imageBlock.imageData = Magick::Image("FlagFoundry/colored_emblems/texture_twocolor.dds");
	auto recolorerTest = EU4::Recolorer::craftTextureImage(imageBlock);
	recolorerTest.write("recolorerTest18.dds");

	const Magick::Image test("recolorerTest18.dds");
	const Magick::Image orig("FlagFoundry/t_twocolor_half2.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color3GraysTwocolorTexture)
{
	EU4::coloredImage imageBlock;
	imageBlock.color3 = commonItems::Color(std::array<int, 3>{255, 215, 0});
	imageBlock.imageData = Magick::Image("FlagFoundry/colored_emblems/texture_twocolor.dds");
	auto recolorerTest = EU4::Recolorer::craftTextureImage(imageBlock);
	recolorerTest.write("recolorerTest19.dds");

	const Magick::Image test("recolorerTest19.dds");
	const Magick::Image orig("FlagFoundry/t_twocolor_gray.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, colorSuiteRecolorsTwocolorTexture)
{
	EU4::coloredImage imageBlock;
	imageBlock.color1 = commonItems::Color(std::array<int, 3>{255, 215, 0});
	imageBlock.color2 = commonItems::Color(std::array<int, 3>{255, 165, 0});
	imageBlock.color3 = commonItems::Color(std::array<int, 3>{50, 180, 200});
	imageBlock.imageData = Magick::Image("FlagFoundry/colored_emblems/texture_twocolor.dds");
	auto recolorerTest = EU4::Recolorer::craftTextureImage(imageBlock);
	recolorerTest.write("recolorerTest20.dds");

	const Magick::Image test("recolorerTest20.dds");
	const Magick::Image orig("FlagFoundry/t_twocolor_full.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color1RecolorsThirdOfTricolorTexture)
{
	EU4::coloredImage imageBlock;
	imageBlock.color1 = commonItems::Color(std::array<int, 3>{255, 215, 0});
	imageBlock.imageData = Magick::Image("FlagFoundry/colored_emblems/texture_tricolor.dds");
	auto recolorerTest = EU4::Recolorer::craftTextureImage(imageBlock);
	recolorerTest.write("recolorerTest21.dds");

	const Magick::Image test("recolorerTest21.dds");
	const Magick::Image orig("FlagFoundry/t_tricolor_third.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color2RecolorsSecondThirdOfTriColorTexture)
{
	EU4::coloredImage imageBlock;
	imageBlock.color2 = commonItems::Color(std::array<int, 3>{255, 215, 0});
	imageBlock.imageData = Magick::Image("FlagFoundry/colored_emblems/texture_tricolor.dds");
	auto recolorerTest = EU4::Recolorer::craftTextureImage(imageBlock);
	recolorerTest.write("recolorerTest22.dds");

	const Magick::Image test("recolorerTest22.dds");
	const Magick::Image orig("FlagFoundry/t_tricolor_third2.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, color3RecolorsFinalThirdOfTriColorTexture)
{
	EU4::coloredImage imageBlock;
	imageBlock.color3 = commonItems::Color(std::array<int, 3>{255, 215, 0});
	imageBlock.imageData = Magick::Image("FlagFoundry/colored_emblems/texture_tricolor.dds");
	auto recolorerTest = EU4::Recolorer::craftTextureImage(imageBlock);
	recolorerTest.write("recolorerTest23.dds");

	const Magick::Image test("recolorerTest23.dds");
	const Magick::Image orig("FlagFoundry/t_tricolor_third3.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_RecolorerTests, colorSuiteRecolorsTricolorTexture)
{
	EU4::coloredImage imageBlock;
	imageBlock.color1 = commonItems::Color(std::array<int, 3>{255, 215, 0});  // recoloring to gold.
	imageBlock.color2 = commonItems::Color(std::array<int, 3>{255, 165, 0});  // recoloring to orange.
	imageBlock.color3 = commonItems::Color(std::array<int, 3>{50, 180, 200}); // recoloring to aquamarine.
	imageBlock.imageData = Magick::Image("FlagFoundry/colored_emblems/texture_tricolor.dds");
	auto recolorerTest = EU4::Recolorer::craftTextureImage(imageBlock);
	recolorerTest.write("recolorerTest24.dds");

	const Magick::Image test("recolorerTest24.dds");
	const Magick::Image orig("FlagFoundry/t_tricolor_full.dds");

	ASSERT_TRUE(orig.compare(test));
}
