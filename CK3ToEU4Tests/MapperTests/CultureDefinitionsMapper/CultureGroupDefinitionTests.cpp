#include "../CK3ToEU4/Source/CK3World/Cultures/Culture.h"
#include "../CK3ToEU4/Source/Mappers/CultureDefinitionsMapper/CultureDefiniton.h"
#include "../CK3ToEU4/Source/Mappers/CultureDefinitionsMapper/CultureGroupDefinition.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::ElementsAre;

TEST(Mappers_CultureGroupDefinitionTests, nameCanBeSet)
{
	std::stringstream input;
	const mappers::CultureGroupDefinition group("name", input);

	EXPECT_EQ("name", group.getName());
}

TEST(Mappers_CultureGroupDefinitionTests, culturesCanBeLoaded)
{
	std::stringstream input;
	input << "someculture = {}\n";
	input << "otherculture = {}\n";
	const mappers::CultureGroupDefinition group("name", input);

	EXPECT_EQ(2, group.getCultures().size());
	EXPECT_TRUE(group.getCultures().contains("someculture"));
	EXPECT_TRUE(group.getCultures().contains("otherculture"));
}

TEST(Mappers_CultureGroupDefinitionTests, culturesCanBePinged)
{
	std::stringstream input;
	input << "someculture = {}\n";
	input << "otherculture = {}\n";
	const mappers::CultureGroupDefinition group("name", input);

	EXPECT_TRUE(group.containsCulture("someculture"));
	EXPECT_TRUE(group.containsCulture("otherculture"));
}

TEST(Mappers_CultureGroupDefinitionTests, culturesPingMismatchReturnsFalse)
{
	std::stringstream input;
	input << "someculture = {}\n";
	input << "otherculture = {}\n";
	const mappers::CultureGroupDefinition group("name", input);

	EXPECT_FALSE(group.containsCulture("missingculture"));
}

TEST(Mappers_CultureGroupDefinitionTests, culturesCanBeMerged)
{
	std::stringstream input;
	input << "someculture = {\n";
	input << "\tmale_names = { bob }\n";
	input << "\tfemale_names = { boba }\n";
	input << "\tdynasty_names = { bobby }\n";
	input << "}\n";
	mappers::CultureGroupDefinition group("name", input);

	std::stringstream input2;
	input2 << "male_names = { jon }\n";
	input2 << "female_names = { jona }\n";
	input2 << "dynasty_names = { jonny }\n";
	const auto sameCulture = std::make_shared<mappers::CultureDefinition>(input2);

	group.mergeCulture("someculture", sameCulture);

	const auto someCulture = group.getCultures().find("someculture")->second;

	EXPECT_EQ(2, someCulture->getMaleNames().size());
	EXPECT_THAT(someCulture->getMaleNames(), ElementsAre("bob", "jon"));
	EXPECT_EQ(2, someCulture->getFemaleNames().size());
	EXPECT_THAT(someCulture->getFemaleNames(), ElementsAre("boba", "jona"));
	EXPECT_EQ(2, someCulture->getDynastyNames().size());
	EXPECT_THAT(someCulture->getDynastyNames(), ElementsAre("bobby", "jonny"));
}

TEST(Mappers_CultureGroupDefinitionTests, unmergeableCulturesAreAddedToGroup)
{
	std::stringstream input;
	input << "someculture = {\n";
	input << "\tmale_names = { bob }\n";
	input << "\tfemale_names = { boba }\n";
	input << "\tdynasty_names = { bobby }\n";
	input << "}\n";
	mappers::CultureGroupDefinition group("name", input);

	std::stringstream input2;
	input2 << "male_names = { jon }\n";
	input2 << "female_names = { jona }\n";
	input2 << "dynasty_names = { jonny }\n";
	const auto otherCulture = std::make_shared<mappers::CultureDefinition>(input2);

	group.mergeCulture("otherculture", otherCulture);

	EXPECT_EQ(2, group.getCultures().size());
	EXPECT_TRUE(group.getCultures().contains("someculture"));
	EXPECT_TRUE(group.getCultures().contains("otherculture"));
}

TEST(Mappers_CultureGroupTests, cultureGroupCanBeOutput)
{
	std::stringstream input;
	input << "someculture = {\n";
	input << "\tmale_names = { Bob Jon }\n";
	input << "\tfemale_names = { Bobby Johnny }\n";
	input << "\tdynasty_names = { Bobby2 Johnny2 }\n";
	input << "}\n";
	mappers::CultureGroupDefinition group("someculturegroup", input);

	// before output, need to assign a ck3 culture to backend and mark it dynamic
	const auto& someCulture = group.getCulture("someculture");
	auto ck3Culture = std::make_shared<CK3::Culture>();
	ck3Culture->setDynamic();
	someCulture->setSourceCulture(ck3Culture);

	std::stringstream output;
	output << "\tsomeculture = {\n";
	output << "\t\tmale_names = { \"Bob\" \"Jon\" }\n";
	output << "\t\tfemale_names = { \"Bobby\" \"Johnny\" }\n";
	output << "\t\tdynasty_names = { \"Bobby2\" \"Johnny2\" }\n";
	output << "\t}\n";

	std::stringstream actual;
	actual << group;

	EXPECT_EQ(output.str(), actual.str());
}
