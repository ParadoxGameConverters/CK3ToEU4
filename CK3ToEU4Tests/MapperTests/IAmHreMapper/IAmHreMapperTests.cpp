#include "../../CK3ToEU4/Source/Mappers/IAmHreMapper/IAmHreMapper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_IAmHreMapperTests, hreDefaultsToBlank)
{
	std::stringstream input;

	const mappers::IAmHreMapper theMapper(input);

	ASSERT_TRUE(theMapper.getHRE().empty());
}

TEST(Mappers_IAmHreMapperTests, hreCanBeSet)
{
	std::stringstream input;
	input << "hre_mechanics = e_rome\n";

	const mappers::IAmHreMapper theMapper(input);

	ASSERT_EQ(theMapper.getHRE(), "e_rome");
}
