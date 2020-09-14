#include "../../CK3toEU4/Source/CK3World/Characters/Character.h"
#include "../../CK3toEU4/Source/Mappers/RulerPersonalitiesMapper/RulerPersonalitiesMapper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_RulerPersonalitiesMapperTests, mappingsDefaultToBlank)
{
	std::stringstream input;
	const mappers::RulerPersonalitiesMapper mapper(input);

	ASSERT_TRUE(mapper.getMappings().empty());
}

TEST(Mappers_RulerPersonalitiesMapperTests, mappingsCanBeLoaded)
{
	std::stringstream input;
	input << "personality = {}\n";
	input << "personality2 = {}\n";
	input << "personality3 = {}\n";
	const mappers::RulerPersonalitiesMapper mapper(input);

	ASSERT_FALSE(mapper.getMappings().empty());
	ASSERT_EQ(mapper.getMappings().find("personality")->first, "personality");
	ASSERT_EQ(mapper.getMappings().find("personality2")->first, "personality2");
	ASSERT_EQ(mapper.getMappings().find("personality3")->first, "personality3");
}

TEST(Mappers_RulerPersonalitiesMapperTests, evaluationReturnsAlphabeticallyLastTwoMatchesForBlankCharacter)
{
	std::stringstream input;
	input << "z-personality = {}\n";
	input << "a-personality2 = {}\n";
	input << "p-personality3 = {}\n";
	const mappers::RulerPersonalitiesMapper mapper(input);

	std::stringstream charinput;
	auto newCharacter = std::make_shared<CK3::Character>(charinput, 1);
	auto charPair = std::pair(1, newCharacter);

	auto returned = mapper.evaluatePersonalities(charPair);

	ASSERT_FALSE(returned.empty());
	ASSERT_EQ(returned.count("z-personality"), 1);
	ASSERT_EQ(returned.count("p-personality3"), 1);
	ASSERT_EQ(returned.size(), 2);
}

TEST(Mappers_RulerPersonalitiesMapperTests, evaluationReturnsEmptyForNoRulesAndBlankCharacter)
{
	std::stringstream input;
	const mappers::RulerPersonalitiesMapper mapper(input);

	std::stringstream charinput;
	auto newCharacter = std::make_shared<CK3::Character>(charinput, 1);
	auto charPair = std::pair(1, newCharacter);

	auto returned = mapper.evaluatePersonalities(charPair);

	ASSERT_TRUE(returned.empty());
}

TEST(Mappers_RulerPersonalitiesMapperTests, evaluationReturnsEmptyForNoRulesAndLoadedCharacter)
{
	std::stringstream input;
	const mappers::RulerPersonalitiesMapper mapper(input);

	std::stringstream charinput;
	auto newCharacter = std::make_shared<CK3::Character>(charinput, 1);
	auto charPair = std::pair(1, newCharacter);
	std::map<int, std::string> traits = {{1, "trait1"}, {2, "trait2"}};
	newCharacter->loadTraits(traits);

	auto returned = mapper.evaluatePersonalities(charPair);

	ASSERT_TRUE(returned.empty());
}

TEST(Mappers_RulerPersonalitiesMapperTests, evaluationReturnsTwoBestPersonalities)
{
	std::stringstream input;
	input << "z-personality = { trait3 = 99 }\n";
	input << "a-personality = { trait3 = -99 }\n";
	input << "p-personality = { trait3 = 0}\n";
	input << "k-personality = { trait3 = -50}\n";
	input << "c-personality = { trait3 = 50}\n";
	const mappers::RulerPersonalitiesMapper mapper(input);

	std::stringstream charinput;
	auto newCharacter = std::make_shared<CK3::Character>(charinput, 1);
	auto charPair = std::pair(1, newCharacter);
	std::map<int, std::string> traits = {{1, "trait1"}, {2, "trait2"}, {3, "trait3"}};
	newCharacter->loadTraits(traits);

	auto returned = mapper.evaluatePersonalities(charPair);

	ASSERT_EQ(returned.size(), 2);
	ASSERT_EQ(returned.count("z-personality"), 1); // score 99
	ASSERT_EQ(returned.count("c-personality"), 1); // score 50
}
