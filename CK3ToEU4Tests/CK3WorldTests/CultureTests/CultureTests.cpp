#include "../CK3toEU4/Source/CK3World/Cultures/Culture.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_CultureTests, cultureIDLoads)
{
	std::stringstream input;
	const CK3::Culture culture(input, 42);

	ASSERT_EQ(42, culture.getID());
}

TEST(CK3World_CultureTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::Culture culture(input, 1);

	ASSERT_TRUE(culture.getName().empty());
}

TEST(CK3World_CultureTests, culturePrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "culture_template=\"akan\"\n";

	const CK3::Culture culture(input, 42);

	ASSERT_EQ("akan", culture.getTemplate());
}
