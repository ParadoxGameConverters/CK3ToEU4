#include "../CK3toEU4/Source/Mappers/ShatterEmpiresMapper/ShatterEmpiresMapper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_ShatterEmpiresMapperTests, empiresDefaultToBlank)
{
	std::stringstream input;

	const mappers::ShatterEmpiresMapper theMapper(input);

	ASSERT_TRUE(theMapper.getEmpires().empty());
}

TEST(Mappers_ShatterEmpiresMapperTests, empiresCanBeLoaded)
{
	std::stringstream input;
	input << "shatter_empires = { e_byzantium e_rome e_rome e_byzantium e_hispania }\n";

	const mappers::ShatterEmpiresMapper theMapper(input);

	ASSERT_EQ(3, theMapper.getEmpires().size());
	ASSERT_TRUE(theMapper.getEmpires().count("e_rome"));
	ASSERT_TRUE(theMapper.getEmpires().count("e_byzantium"));
	ASSERT_TRUE(theMapper.getEmpires().count("e_hispania"));
}

TEST(Mappers_ShatterEmpiresMapperTests, empireCanBePinged)
{
	std::stringstream input;
	input << "shatter_empires = { e_byzantium e_rome e_rome e_byzantium e_hispania }\n";

	const mappers::ShatterEmpiresMapper theMapper(input);

	ASSERT_TRUE(theMapper.isEmpireShatterable("e_rome"));
	ASSERT_FALSE(theMapper.isEmpireShatterable("e_india"));
}
