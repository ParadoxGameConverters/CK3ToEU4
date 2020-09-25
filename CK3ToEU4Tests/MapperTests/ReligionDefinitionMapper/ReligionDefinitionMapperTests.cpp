#include "../../CK3toEU4/Source/Mappers/ReligionDefinitionMapper/ReligionDefinitionMapper.h"
#include "../../CK3toEU4/Source/Mappers/ReligionDefinitionMapper/ReligionDefinitionMapping.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_ReligionDefinitionMapperTests, nonMatchGivesEmptyOptional)
{
	std::stringstream input;
	input << "definition = { country={} province={something or other} }";
	std::stringstream input2;
	const mappers::ReligionDefinitionMapper theMapper(input, input2);

	const auto& definition = theMapper.getDefinition("nonsense");
	ASSERT_FALSE(definition);
}

TEST(Mappers_ReligionDefinitionMapperTests, matchReturnsDefinition)
{
	std::stringstream input;
	input << "definition = { country={} province={something or other} }";
	std::stringstream input2;
	const mappers::ReligionDefinitionMapper theMapper(input, input2);

	const auto& definition = theMapper.getDefinition("definition");
	ASSERT_EQ("something or other", definition->getProvince());
	ASSERT_TRUE(definition->getCountry().empty());
}

TEST(Mappers_ReligionDefinitionMapperTests, nonMatchStaticGivesEmptyOptional)
{
	std::stringstream input;
	std::stringstream input2;
	input << "religion = {This is a very large static blob.\nIt is substantial and full of horrors.}";
	const mappers::ReligionDefinitionMapper theMapper(input, input2);

	const auto& religion = theMapper.getStaticBlob("nonsense");
	ASSERT_FALSE(religion);
}

TEST(Mappers_ReligionDefinitionMapperTests, matchStaticReturnsBlob)
{
	std::stringstream input;
	std::stringstream input2;
	input2 << "religion = {This is a very large static blob.\nIt is substantial and full of horrors.}";
	const mappers::ReligionDefinitionMapper theMapper(input, input2);

	const auto& religion = theMapper.getStaticBlob("religion");
	ASSERT_EQ("This is a very large static blob.\nIt is substantial and full of horrors.", *religion);
}
