#include "../../CK3toEU4/Source/Mappers/RulerPersonalitiesMapper/RulerPersonalitiesMapping.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_RulerPersonalitiesMappingTests, traitsDefaultToBlank)
{
	std::stringstream input;
	input << "= {}";

	const mappers::RulerPersonalitiesMapping theMapping(input);

	ASSERT_TRUE(theMapping.getTraits().empty());
}

TEST(Mappers_RulerPersonalitiesMappingTests, traitsCanBeLoaded)
{
	std::stringstream input;
	input << "= { trait = 0 trait2 = -9 trait3 = 7 }";

	const mappers::RulerPersonalitiesMapping theMapping(input);

	ASSERT_EQ(3, theMapping.getTraits().size(), 3);
	ASSERT_EQ(0, theMapping.getTraits().find("trait")->second);
	ASSERT_EQ(-9, theMapping.getTraits().find("trait2")->second);
	ASSERT_EQ(7, theMapping.getTraits().find("trait3")->second);
}

TEST(Mappers_RulerPersonalitiesMappingTests, incomingTraitsScoreZeroForNoLoadedTraits)
{
	std::stringstream input;

	const mappers::RulerPersonalitiesMapping theMapping(input);

	const std::set<std::string> incomingTraits = {"trait1", "trait2", "trait3"};

	ASSERT_EQ(0, theMapping.evaluatePersonality(incomingTraits));
}

TEST(Mappers_RulerPersonalitiesMappingTests, incomingTraitsScoreZeroForNoIncomingTraits)
{
	std::stringstream input;
	input << "= { trait = 0 trait2 = -9 trait3 = 7 }";
	const mappers::RulerPersonalitiesMapping theMapping(input);

	const std::set<std::string> incomingTraits;

	ASSERT_EQ(0, theMapping.evaluatePersonality(incomingTraits));
}

TEST(Mappers_RulerPersonalitiesMappingTests, incomingTraitCanMatchAndScore)
{
	std::stringstream input;
	input << "= { trait = 0 trait2 = -9 trait3 = 7 }";
	const mappers::RulerPersonalitiesMapping theMapping(input);

	const std::set<std::string> incomingTraits = {"trait1", "trait7", "trait2"};

	ASSERT_EQ(-9, theMapping.evaluatePersonality(incomingTraits));
}

TEST(Mappers_RulerPersonalitiesMappingTests, multipleTraitsCanMatchAndScore)
{
	std::stringstream input;
	input << "= { trait = 0 trait2 = -9 trait3 = 7 }";
	const mappers::RulerPersonalitiesMapping theMapping(input);

	const std::set<std::string> incomingTraits = {"trait", "trait2", "trait3"}; // 0-9+7=-2

	ASSERT_EQ(-2, theMapping.evaluatePersonality(incomingTraits));
}
