#include "../CK3toEU4/Source/EU4World/FlagFoundry/FlagCrafter.h"
#include "../CK3toEU4/Source/EU4World/FlagFoundry/Warehouse.h"
#include "../CK3toEU4/Source/CK3World/CoatsOfArms/CoatOfArms.h"
#include "gtest/gtest.h"
#include "Magick++.h"
#include <sstream>

TEST(EU4World_FlagCrafterTests, test)
{
	const auto warehouse = std::make_shared<EU4::Warehouse>();
	warehouse->loadImageFolder("FlagFoundry/");
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

	auto result = flagCrafter.craftFlagFromCoA(coa);
	result.write("result-coa.dds");

	ASSERT_FALSE(result.isValid());
}

TEST(EU4World_FlagCrafterTests, test2)
{
	const auto warehouse = std::make_shared<EU4::Warehouse>();
	warehouse->loadImageFolder("FlagFoundry/");
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

	auto result = flagCrafter.craftFlagFromCoA(coa);
	result.write("result-coa2.dds");

	ASSERT_FALSE(result.isValid());
}

TEST(EU4World_FlagCrafterTests, test3)
{
	const auto warehouse = std::make_shared<EU4::Warehouse>();
	warehouse->loadImageFolder("FlagFoundry/");
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

	auto result = flagCrafter.craftFlagFromCoA(coa);
	result.write("result-coa3.dds");

	ASSERT_FALSE(result.isValid());
}
