#include "../CK3toEU4/Source/CK3World/CoatsOfArms/CoatOfArms.h"
#include "../CK3toEU4/Source/EU4World/FlagFoundry/FlagCrafter.h"
#include "../CK3toEU4/Source/EU4World/FlagFoundry/Warehouse.h"
#include "Magick++.h"
#include "gtest/gtest.h"
#include <fstream>

TEST(EU4World_FlagCrafterTests, emblemOverlayOnPattern)
{
	const auto warehouse = std::make_shared<EU4::Warehouse>();
	warehouse->loadImageFolders({"FlagFoundry/"});
	EU4::FlagCrafter flagCrafter;
	flagCrafter.loadWarehouse(warehouse);

	std::stringstream input;
	input << "pattern = pattern_solid.dds\n";
	input << "color1 = { 94 193 11 }\n";
	input << "color2 = { 12 45 89 }\n"; // This is typical PDX erroneous input; color2 does not apply to solid patterns
	input << "colored_emblem = {\n";
	input << "texture = texture_solid.dds\n";
	input << "color1 = { 250 99 243 }\n";
	input << "}\n";
	const CK3::CoatOfArms coa(input, 79);

	auto flagCrafterTest = flagCrafter.craftFlagFromCoA(coa);
	flagCrafterTest.write("flagCrafterTest1.dds");

	const Magick::Image test("flagCrafterTest1.dds");
	const Magick::Image orig("FlagFoundry/flagtest1.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_FlagCrafterTests, multicolorEmblemsOverlayOnMulticolorPattern)
{
	const auto warehouse = std::make_shared<EU4::Warehouse>();
	warehouse->loadImageFolders({"FlagFoundry/"});
	EU4::FlagCrafter flagCrafter;
	flagCrafter.loadWarehouse(warehouse);

	std::stringstream input;
	input << "pattern = pattern_tricolor.dds\n";
	input << "color1 = { 94 193 11 }\n";
	input << "color2 = { 12 45 89 }\n";
	input << "color3 = { 12 180 135 }\n";
	input << "colored_emblem = {\n";
	input << "texture = texture_tricolor.dds\n";
	input << "color1 = { 250 99 243 }\n";
	input << "color2 = { 0 199 245 }\n";
	input << "color3 = { 150 99 143 }\n";
	input << "}\n";
	const CK3::CoatOfArms coa(input, 79);

	auto flagCrafterTest = flagCrafter.craftFlagFromCoA(coa);
	flagCrafterTest.write("flagCrafterTest2.dds");

	const Magick::Image test("flagCrafterTest2.dds");
	const Magick::Image orig("FlagFoundry/flagtest2.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_FlagCrafterTests, emblemInstanceStretchingAndPositioning)
{
	laFabricaDeColor.clear();
	laFabricaDeColor.addNamedColor("white", commonItems::Color(std::array<int, 3>{255, 255, 255}));
	laFabricaDeColor.addNamedColor("red", commonItems::Color(std::array<int, 3>{255, 0, 0}));

	const auto warehouse = std::make_shared<EU4::Warehouse>();
	warehouse->loadImageFolders({"FlagFoundry/"});
	EU4::FlagCrafter flagCrafter;
	flagCrafter.loadWarehouse(warehouse);

	std::ifstream input("FlagFoundry/magyar_flag.txt");
	const CK3::CoatOfArms coa(input, 79);

	auto flagCrafterTest = flagCrafter.craftFlagFromCoA(coa);
	flagCrafterTest.write("flagCrafterTest7.dds");

	const Magick::Image test("flagCrafterTest7.dds");
	const Magick::Image orig("FlagFoundry/flagtest7.dds");

	laFabricaDeColor.clear();
	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_FlagCrafterTests, emblemInstancePositioningRotationAndScaling)
{
	const auto warehouse = std::make_shared<EU4::Warehouse>();
	warehouse->loadImageFolders({"FlagFoundry/"});
	EU4::FlagCrafter flagCrafter;
	flagCrafter.loadWarehouse(warehouse);

	std::stringstream input;
	input << "pattern = pattern_tricolor.dds\n";
	input << "color1 = { 94 193 11 }\n";
	input << "color2 = { 12 45 89 }\n";
	input << "color3 = { 12 180 135 }\n";
	input << "colored_emblem = {\n";
	input << "texture = texture_tricolor.dds\n";
	input << "color1 = { 250 99 100 }\n";
	input << "color2 = { 100 199 245 }\n";
	input << "color3 = { 150 99 143 }\n";
	input << "instance = {\n";
	input << "position = { 0.25 0.25 }\n";
	input << "rotation = 0\n";
	input << "scale = { 0.5 0.5 }\n";
	input << "}\n";
	input << "instance = {\n";
	input << "position = { 0.75 0.25 }\n";
	input << "rotation = 0\n";
	input << "scale = { 0.5 0.5 }\n";
	input << "}\n";
	input << "instance = {\n";
	input << "position = { 0.25 0.75 }\n";
	input << "rotation = 0\n";
	input << "scale = { 0.5 0.5 }\n";
	input << "}\n";
	input << "instance = {\n";
	input << "position = { 0.75 0.75 }\n";
	input << "rotation = 45\n";
	input << "scale = { 0.5 0.5 }\n";
	input << "}\n";
	input << "}\n";
	const CK3::CoatOfArms coa(input, 79);

	auto flagCrafterTest = flagCrafter.craftFlagFromCoA(coa);
	flagCrafterTest.write("flagCrafterTest3.dds");

	const Magick::Image test("flagCrafterTest3.dds");
	const Magick::Image orig("FlagFoundry/flagtest3.dds");

	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_FlagCrafterTests, subcoatPositioningRotationAndScaling)
{
	laFabricaDeColor.clear();
	laFabricaDeColor.addNamedColor("white", commonItems::Color(std::array<int, 3>{255, 255, 255}));
	laFabricaDeColor.addNamedColor("blue", commonItems::Color(std::array<int, 3>{0, 0, 255}));
	laFabricaDeColor.addNamedColor("red", commonItems::Color(std::array<int, 3>{255, 0, 0}));
	laFabricaDeColor.addNamedColor("yellow", commonItems::Color(std::array<int, 3>{255, 255, 0}));

	const auto warehouse = std::make_shared<EU4::Warehouse>();
	warehouse->loadImageFolders({"FlagFoundry/"});
	EU4::FlagCrafter flagCrafter;
	flagCrafter.loadWarehouse(warehouse);

	std::ifstream input("FlagFoundry/flag.txt");
	const CK3::CoatOfArms coa(input, 79);

	auto flagCrafterTest = flagCrafter.craftFlagFromCoA(coa);
	flagCrafterTest.write("flagCrafterTest4.dds");

	const Magick::Image test("flagCrafterTest4.dds");
	const Magick::Image orig("FlagFoundry/flagtest4.dds");

	laFabricaDeColor.clear();
	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_FlagCrafterTests, simpleMaskTest)
{
	const auto warehouse = std::make_shared<EU4::Warehouse>();
	warehouse->loadImageFolders({"FlagFoundry/"});
	EU4::FlagCrafter flagCrafter;
	flagCrafter.loadWarehouse(warehouse);

	std::stringstream input;
	input << "pattern = pattern_split.dds\n";
	input << "color1 = { 200 100 100 }\n";
	input << "color2 = { 100 100 200 }\n";
	input << "colored_emblem = {\n";
	input << "texture = texture_solid.dds\n";
	input << "color1 = { 100 200 100 }\n";
	input << "mask = { 1 0 0 }\n";
	input << "}\n";
	const CK3::CoatOfArms coa(input, 79);

	auto flagCrafterTest = flagCrafter.craftFlagFromCoA(coa);
	flagCrafterTest.write("flagCrafterTest5.dds");

	const Magick::Image test("flagCrafterTest5.dds");
	const Magick::Image orig("FlagFoundry/flagtest5.dds");

	laFabricaDeColor.clear();
	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_FlagCrafterTests, dodgeMaskTest)
{
	const auto warehouse = std::make_shared<EU4::Warehouse>();
	warehouse->loadImageFolders({"FlagFoundry/"});
	EU4::FlagCrafter flagCrafter;
	flagCrafter.loadWarehouse(warehouse);

	std::stringstream input;
	input << "pattern = pattern_split.dds\n";
	input << "color1 = { 200 100 100 }\n";
	input << "color2 = { 100 100 200 }\n";
	input << "colored_emblem = {\n";
	input << "texture = texture_solid.dds\n";
	input << "color1 = { 100 200 100 }\n";
	input << "mask = { 0 2 0 }\n";
	input << "instance = {\n";
	input << "position = { 0.6 0.2 }\n";
	input << "rotation = -45\n";
	input << "scale = { 0.5 0.5 }\n";
	input << "}\n";
	input << "}\n";
	const CK3::CoatOfArms coa(input, 79);

	auto flagCrafterTest = flagCrafter.craftFlagFromCoA(coa);
	flagCrafterTest.write("flagCrafterTest6.dds");

	const Magick::Image test("flagCrafterTest6.dds");
	const Magick::Image orig("FlagFoundry/flagtest6.dds");

	laFabricaDeColor.clear();
	ASSERT_TRUE(orig.compare(test));
}

TEST(EU4World_FlagCrafterTests, almeriaTheFinalTestFromHell)
{
	laFabricaDeColor.clear();
	laFabricaDeColor.addNamedColor("white",
		 commonItems::Color(std::array<float, 3>{static_cast<float>(0.08), static_cast<float>(0.1), static_cast<float>(0.75)}));
	laFabricaDeColor.addNamedColor("blue", commonItems::Color(std::array<float, 3>{static_cast<float>(0.58), static_cast<float>(0.8), static_cast<float>(0.4)}));
	laFabricaDeColor.addNamedColor("red", commonItems::Color(std::array<float, 3>{static_cast<float>(0.02), static_cast<float>(0.8), static_cast<float>(0.45)}));
	laFabricaDeColor.addNamedColor("yellow",
		 commonItems::Color(std::array<float, 3>{static_cast<float>(0.1), static_cast<float>(0.75), static_cast<float>(0.75)}));
	laFabricaDeColor.addNamedColor("purple",
		 commonItems::Color(std::array<float, 3>{static_cast<float>(0.9), static_cast<float>(0.7), static_cast<float>(0.35)}));

	const auto warehouse = std::make_shared<EU4::Warehouse>();
	warehouse->loadImageFolders({"FlagFoundry/"});
	EU4::FlagCrafter flagCrafter;
	flagCrafter.loadWarehouse(warehouse);

	std::ifstream input("FlagFoundry/almeriaTestFromHell.txt");
	const CK3::CoatOfArms coa(input, 79);

	auto flagCrafterTest = flagCrafter.craftFlagFromCoA(coa);
	flagCrafterTest.write("flagCrafterTest-almeria.dds");

	const Magick::Image test("flagCrafterTest-almeria.dds");
	const Magick::Image orig("FlagFoundry/flagtest-almeria.dds");

	laFabricaDeColor.clear();
	ASSERT_TRUE(orig.compare(test));
}
