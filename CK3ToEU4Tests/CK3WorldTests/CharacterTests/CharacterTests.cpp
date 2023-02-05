#include "../CK3ToEU4/Source/CK3World/Characters/Character.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_CharacterTests, characterIDLoads)
{
	std::stringstream input;
	const CK3::Character character(input, 42);

	ASSERT_EQ(42, character.getID());
}

TEST(CK3World_CharacterTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::Character character(input, 1);

	ASSERT_TRUE(character.getName().empty());
	ASSERT_EQ(date("1.1.1"), character.getBirthDate());
	ASSERT_FALSE(character.getCulture());
	ASSERT_FALSE(character.getFaith());
	ASSERT_FALSE(character.getHouse().first);
	ASSERT_FALSE(character.getSkills().martial);
	ASSERT_FALSE(character.getSkills().diplomacy);
	ASSERT_FALSE(character.getSkills().stewardship);
	ASSERT_FALSE(character.getSkills().learning);
	ASSERT_FALSE(character.getSkills().intrigue);
	ASSERT_TRUE(character.getTraits().empty());
	ASSERT_EQ(0.0f, character.getPiety());
	ASSERT_EQ(0.0f, character.getPrestige());
	ASSERT_EQ(0.0f, character.getGold());
	ASSERT_TRUE(character.getClaims().empty());
	ASSERT_FALSE(character.getEmployer());
	ASSERT_FALSE(character.isKnight());
	ASSERT_FALSE(character.isCouncilor());
	ASSERT_FALSE(character.isFemale());
	ASSERT_FALSE(character.getSpouse());
	ASSERT_FALSE(character.getCharacterDomain());
	ASSERT_FALSE(character.isDead());
}

TEST(CK3World_CharacterTests, characterPrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "first_name = \"bob spongepants\"\n";
	input << "birth = 9.9.9\n";
	input << "culture = 1\n";
	input << "faith = 2\n";
	input << "dynasty_house = 3\n";
	input << "skill = { 11 12 13 14 15 16 }\n";
	input << "traits = { 21 22 23 24 25 26 26 26 }\n";
	input << "female = yes\n";
	input << "dead_data = { irrelevant details }\n";

	const CK3::Character character(input, 42);

	ASSERT_EQ("bob spongepants", character.getName());
	ASSERT_EQ(date("9.9.9"), character.getBirthDate());
	ASSERT_EQ(1, character.getCulture()->first);
	ASSERT_EQ(2, character.getFaith()->first);
	ASSERT_EQ(3, character.getHouse().first);
	ASSERT_EQ(11, character.getSkills().diplomacy);
	ASSERT_EQ(12, character.getSkills().martial);
	ASSERT_EQ(13, character.getSkills().stewardship);
	ASSERT_EQ(14, character.getSkills().intrigue);
	ASSERT_EQ(15, character.getSkills().learning);
	ASSERT_EQ(6, character.getTraits().size());
	ASSERT_EQ(1, character.getTraits().count(26));
	ASSERT_EQ(1, character.getTraits().count(21));
	ASSERT_TRUE(character.isFemale());
	ASSERT_TRUE(character.isDead());
}

TEST(CK3World_CharacterTests, characterAliveDataCanBeLoaded)
{
	std::stringstream input;
	input << "alive_data = {\n";
	input << "\tpiety = {\n";
	input << "\t\taccumulated = 100.01\n";
	input << "\t}\n";
	input << "\tprestige = {\n";
	input << "\t\taccumulated = 101.02\n";
	input << "\t}\n";
	input << "\tgold = 103.02\n";
	input << "\tclaim = { { title = 1 } { title = 3 } { title = 5 } }\n";
	input << "}";

	const CK3::Character character(input, 42);

	ASSERT_NEAR(100.01, character.getPiety(), 0.001);
	ASSERT_NEAR(101.02, character.getPrestige(), 0.001);
	ASSERT_NEAR(103.02, character.getGold(), 0.001);
	ASSERT_EQ(3, character.getClaims().size());
	ASSERT_EQ(1, character.getClaims().count(1));
	ASSERT_EQ(1, character.getClaims().count(3));
	ASSERT_EQ(1, character.getClaims().count(5));
}

TEST(CK3World_CharacterTests, characterFamilyDataCanBeLoaded)
{
	std::stringstream input;
	input << "family_data = {\n";
	input << "\tprimary_spouse = 17\n";
	input << "}";

	const CK3::Character character(input, 42);

	ASSERT_EQ(17, character.getSpouse()->first);
}

TEST(CK3World_CharacterTests, characterCourtDataCanBeLoaded)
{
	std::stringstream input;
	input << "court_data = {\n";
	input << "\temployer = 27\n";
	input << "\tknight = yes\n";
	input << "\tcouncil_task = 7248\n";
	input << "}";

	const CK3::Character character(input, 42);

	ASSERT_EQ(27, character.getEmployer()->first);
	ASSERT_TRUE(character.isKnight());
	ASSERT_TRUE(character.isCouncilor());
}

TEST(CK3World_CharacterTests, characterDomainCanBeLoaded)
{
	std::stringstream input;
	input << "landed_data = {\n";
	input << "\tis_powerful_vassal = yes\n";
	input << "}";

	const CK3::Character character(input, 42);

	ASSERT_TRUE(character.getCharacterDomain()->isPowerfulVassal());
}
