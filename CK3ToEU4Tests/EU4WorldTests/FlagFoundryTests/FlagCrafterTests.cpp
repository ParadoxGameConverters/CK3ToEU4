#include "../CK3toEU4/Source/EU4World/FlagFoundry/FlagCrafter.h"
#include "../CK3toEU4/Source/EU4World/FlagFoundry/Warehouse.h"
#include "gtest/gtest.h"
#include "Magick++.h"
#include <sstream>

TEST(EU4World_FlagCrafterTests, test)
{
	const auto warehouse = std::make_shared<EU4::Warehouse>();
	warehouse->loadImageFolder("FlagFoundry/");
	EU4::FlagCrafter flagCrafter;
	flagCrafter.loadWarehouse(warehouse);

	
}
