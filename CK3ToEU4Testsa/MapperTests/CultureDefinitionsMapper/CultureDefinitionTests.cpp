#include "../CK3ToEU4/Source/CK3World/Cultures/Culture.h"
#include "../CK3ToEU4/Source/Mappers/CultureDefinitionsMapper/CultureDefiniton.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::ElementsAre;

TEST(Mappers_CultureDefinitionTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const mappers::CultureDefinition culture(input);

	EXPECT_TRUE(culture.getMaleNames().empty());
	EXPECT_TRUE(culture.getFemaleNames().empty());
	EXPECT_TRUE(culture.getDynastyNames().empty());
}

TEST(Mappers_CultureDefinitionTests, maleNamesCanBeRetrievedAndAdded)
{
	std::stringstream input;
	input << "male_names = { Bob Jon }";
	mappers::CultureDefinition culture(input);

	EXPECT_EQ(2, culture.getMaleNames().size());
	EXPECT_THAT(culture.getMaleNames(), ElementsAre("Bob", "Jon"));

	const std::vector<std::string> moreNames = {"Dod", "Kro"};

	culture.addMaleNames(moreNames);

	EXPECT_EQ(4, culture.getMaleNames().size());
	EXPECT_THAT(culture.getMaleNames(), ElementsAre("Bob", "Jon", "Dod", "Kro"));
}

TEST(Mappers_CultureDefinitionTests, femaleNamesCanBeRetrievedAndAdded)
{
	std::stringstream input;
	input << "female_names = { Bob Jon }";
	mappers::CultureDefinition culture(input);

	EXPECT_EQ(2, culture.getFemaleNames().size());
	EXPECT_THAT(culture.getFemaleNames(), ElementsAre("Bob", "Jon"));

	const std::vector<std::string> moreNames = {"Dod", "Kro"};

	culture.addFemaleNames(moreNames);

	EXPECT_EQ(4, culture.getFemaleNames().size());
	EXPECT_THAT(culture.getFemaleNames(), ElementsAre("Bob", "Jon", "Dod", "Kro"));
}

TEST(Mappers_CultureDefinitionTests, dynastyNamesCanBeRetrievedAndAdded)
{
	std::stringstream input;
	input << "dynasty_names = { Bob Jon }";
	mappers::CultureDefinition culture(input);

	EXPECT_EQ(2, culture.getDynastyNames().size());
	EXPECT_THAT(culture.getDynastyNames(), ElementsAre("Bob", "Jon"));

	const std::vector<std::string> moreNames = {"Dod", "Kro"};

	culture.addDynastyNames(moreNames);

	EXPECT_EQ(4, culture.getDynastyNames().size());
	EXPECT_THAT(culture.getDynastyNames(), ElementsAre("Bob", "Jon", "Dod", "Kro"));
}

TEST(Mappers_CultureDefinitionTests, cultureCanBeOutput)
{
	std::stringstream input;
	input << "male_names = { Bob Jon }\n";
	input << "female_names = { Bobby Johnny }\n";
	input << "dynasty_names = { Bobby2 Johnny2 }\n";
	mappers::CultureDefinition culture(input);

	// before output, need to assign a ck3 culture to backend and mark it dynamic
	auto ck3Culture = std::make_shared<CK3::Culture>();
	ck3Culture->setDynamic();
	culture.setSourceCulture(ck3Culture);

	std::stringstream output;
	output << "\t\tmale_names = { \"Bob\" \"Jon\" }\n";
	output << "\t\tfemale_names = { \"Bobby\" \"Johnny\" }\n";
	output << "\t\tdynasty_names = { \"Bobby2\" \"Johnny2\" }\n";

	std::stringstream actual;
	actual << culture;

	EXPECT_EQ(output.str(), actual.str());
}
