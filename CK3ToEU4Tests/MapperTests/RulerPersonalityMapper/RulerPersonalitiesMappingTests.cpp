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

	ASSERT_EQ(theMapping.getTraits().size(), 3);
	ASSERT_EQ(theMapping.getTraits().find("trait")->second, 0);
	ASSERT_EQ(theMapping.getTraits().find("trait2")->second, -9);
	ASSERT_EQ(theMapping.getTraits().find("trait3")->second, 7);
}

TEST(Mappers_RulerPersonalitiesMappingTests, incomingTraitsScoreZeroForNoLoadedTraits)
{
	std::stringstream input;

	const mappers::RulerPersonalitiesMapping theMapping(input);

	const std::set<std::string> incomingTraits = {"trait1", "trait2", "trait3"};

	ASSERT_EQ(theMapping.evaluatePersonality(incomingTraits), 0);
}

TEST(Mappers_RulerPersonalitiesMappingTests, incomingTraitsScoreZeroForNoIncomingTraits)
{
	std::stringstream input;
	input << "= { trait = 0 trait2 = -9 trait3 = 7 }";
	const mappers::RulerPersonalitiesMapping theMapping(input);

	const std::set<std::string> incomingTraits;

	ASSERT_EQ(theMapping.evaluatePersonality(incomingTraits), 0);
}

TEST(Mappers_RulerPersonalitiesMappingTests, incomingTraitCanMatchAndScore)
{
	std::stringstream input;
	input << "= { trait = 0 trait2 = -9 trait3 = 7 }";
	const mappers::RulerPersonalitiesMapping theMapping(input);

	const std::set<std::string> incomingTraits = {"trait1", "trait7", "trait2"};

	ASSERT_EQ(theMapping.evaluatePersonality(incomingTraits), -9);
}

TEST(Mappers_RulerPersonalitiesMappingTests, multipleTraitsCanMatchAndScore)
{
	std::stringstream input;
	input << "= { trait = 0 trait2 = -9 trait3 = 7 }";
	const mappers::RulerPersonalitiesMapping theMapping(input);

	const std::set<std::string> incomingTraits = {"trait", "trait2", "trait3"}; // 0-9+7=-2

	ASSERT_EQ(theMapping.evaluatePersonality(incomingTraits), -2);
}
