#include "../../CK3toEU4/Source/Mappers/PrimaryTagMapper/PrimaryTagCulture.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_PrimaryTagCultureTests, PrimaryTagDefaultsToNull)
{
	std::stringstream input;

	const mappers::PrimaryTagCulture theMapping(input);

	ASSERT_FALSE(theMapping.getTag());
}

TEST(Mappers_PrimaryTagCultureTests, PrimaryTagCanBeSet)
{
	std::stringstream input;
	input << "primary = TAG";

	const mappers::PrimaryTagCulture theMapping(input);

	ASSERT_EQ("TAG", theMapping.getTag());
}
