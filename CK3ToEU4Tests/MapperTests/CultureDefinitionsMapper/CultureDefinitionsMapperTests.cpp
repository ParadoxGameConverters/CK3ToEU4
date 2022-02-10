#include "../CK3toEU4/Source/Mappers/CultureDefinitionsMapper/CultureDefinitionsMapper.h"
#include "../CK3toEU4/Source/Mappers/CultureDefinitionsMapper/CultureDefiniton.h"
#include "../CK3toEU4/Source/Mappers/CultureDefinitionsMapper/CultureGroupDefinition.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

#include "../../../CK3toEU4/Source/CK3World/Cultures/Culture.h"
using testing::ElementsAre;

TEST(Mappers_CultureDefinitionMapperTests, cultureGroupsCanBeLoaded)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureB = {} }\n";
	const mappers::CultureDefinitionsMapper groups(input);

	EXPECT_EQ(2, groups.getCultureGroupsMap().size());
	EXPECT_TRUE(groups.getCultureGroupsMap().contains("groupA"));
	EXPECT_TRUE(groups.getCultureGroupsMap().contains("groupB"));
}

TEST(Mappers_CultureDefinitionMapperTests, groupCanBeRetrieved)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureB = {} }\n";
	const mappers::CultureDefinitionsMapper groups(input);

	const auto& groupA = groups.getGroupForCulture("cultureA");

	EXPECT_EQ("groupA", groupA->getName());
	EXPECT_TRUE(groupA->containsCulture("cultureA"));
}

TEST(Mappers_CultureDefinitionMapperTests, groupMismatchReturnsNullptr)
{
	std::stringstream input;
	const mappers::CultureDefinitionsMapper groups(input);

	const auto& groupC = groups.getGroupForCulture("cultureC");

	EXPECT_EQ(nullptr, groupC);
}

TEST(Mappers_CultureDefinitionMapperTests, dynamicCultureGroupsCanBeOutput)
{
	std::stringstream input;
	input << "someculturegroup = {\n";
	input << "\tsomeculture = {\n";
	input << "\t\tmale_names = { Bob Jon }\n";
	input << "\t\tfemale_names = { Bobby Johnny }\n";
	input << "\t\tdynasty_names = { Bobby2 Johnny2 }\n";
	input << "\t}\n";
	input << "}\n";
	mappers::CultureDefinitionsMapper groups(input);

	// before output, need to assign a ck3 culture to backend and mark it dynamic
	const auto& someCulture = groups.getCulture("someculture");
	auto ck3Culture = std::make_shared<CK3::Culture>();
	ck3Culture->setDynamic();
	someCulture->setSourceCulture(ck3Culture);


	std::stringstream output;
	output << "someculturegroup = {\n";
	output << "\tsomeculture = {\n";
	output << "\t\tmale_names = { \"Bob\" \"Jon\" }\n";
	output << "\t\tfemale_names = { \"Bobby\" \"Johnny\" }\n";
	output << "\t\tdynasty_names = { \"Bobby2\" \"Johnny2\" }\n";
	output << "\t}\n";
	output << "}\n\n";

	std::stringstream actual;
	actual << groups;

	EXPECT_EQ(output.str(), actual.str());
}
