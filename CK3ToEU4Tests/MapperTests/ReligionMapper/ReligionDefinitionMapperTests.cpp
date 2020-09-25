#include "../../CK3toEU4/Source/Mappers/ReligionMapper/ReligionDefinitionMapper.h"
#include "../../CK3toEU4/Source/Mappers/ReligionMapper/ReligionDefinitionMapping.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_ReligionDefinitionMapperTests, nonMatchGivesEmptyOptional)
{
	std::stringstream input;
	input << "definition = { country={} province={something or other} }";

	const mappers::ReligionDefinitionMapper theMapper(input);

	const auto& definition = theMapper.getDefinition("nonsense");
	ASSERT_FALSE(definition);
}

TEST(Mappers_ReligionDefinitionMapperTests, matchReturnsDefinition)
{
	std::stringstream input;
	input << "definition = { country={} province={something or other} }";
	const mappers::ReligionDefinitionMapper theMapper(input);

	const auto& definition = theMapper.getDefinition("definition");
	ASSERT_EQ("something or other", definition->getProvince());
	ASSERT_TRUE(definition->getCountry().empty());
}
