#include "../../../CK3toEU4/Source/CK3World/Cultures/Culture.h"
#include "../../../CK3toEU4/Source/CK3World/Cultures/Cultures.h"
#include "../../../CK3toEU4/Source/Mappers/CultureMapper/CultureMapper.h"
#include "../CK3toEU4/Source/Mappers/CultureDefinitionsMapper/CultureDefinitionsMapper.h"
#include "../CK3toEU4/Source/Mappers/CultureDefinitionsMapper/CultureDefiniton.h"
#include "../CK3toEU4/Source/Mappers/CultureDefinitionsMapper/CultureGroupDefinition.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
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

TEST(Mappers_CultureDefinitionMapperTests, CultureGroupForCultureNameCanMeRetrieved)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureB = {} }\n";
	const mappers::CultureDefinitionsMapper groups(input);

	const auto& target = groups.getCultureGroupForCultureName("cultureA");

	EXPECT_EQ("groupA", target->getName());
}

TEST(Mappers_CultureDefinitionMapperTests, CultureGroupForCultureNameMismatchReturnsNullptr)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureB = {} }\n";
	const mappers::CultureDefinitionsMapper groups(input);

	const auto& target = groups.getCultureGroupForCultureName("cultureC");

	EXPECT_EQ(nullptr, target);
}

TEST(Mappers_CultureDefinitionMapperTests, CultureGroupForCultureGroupNameCanMeRetrieved)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureB = {} }\n";
	mappers::CultureDefinitionsMapper groups(input);

	const auto& target = groups.getCultureGroup("groupA");

	EXPECT_EQ("groupA", target->getName());
}

TEST(Mappers_CultureDefinitionMapperTests, CultureGroupForCultureGroupNameMismatchSpawnsNewCultureGroup)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureB = {} }\n";
	mappers::CultureDefinitionsMapper groups(input);

	const auto& target = groups.getCultureGroup("cultureC");

	EXPECT_EQ("cultureC", target->getName());
}

TEST(Mappers_CultureDefinitionMapperTests, CultureForCultureNameCanMeRetrieved)
{
	std::stringstream input;
	input << "groupA = { cultureA = { male_names = { bob } }\n";
	input << "groupB = { cultureB = {} }\n";
	const mappers::CultureDefinitionsMapper groups(input);

	const auto& target = groups.getCulture("cultureA");

	EXPECT_EQ("bob", *target->getMaleNames().begin());
}

TEST(Mappers_CultureDefinitionMapperTests, CultureForCultureNameMismatchReturnsNullptr)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureB = {} }\n";
	const mappers::CultureDefinitionsMapper groups(input);

	const auto& target = groups.getCulture("cultureC");

	EXPECT_EQ(nullptr, target);
}

TEST(Mappers_CultureDefinitionMapperTests, BuildingDefsForDynamicCulturesWorks)
{
	// We'll be building a custom definition for a brand new hybrid culture called "dynamic-akan-kru-culture-num1"
	// Building defs for divergent cultures is identical, just has a single instead of 2 incoming components.
	// localizationMapper is dry because it can't help us with dynamic culture names.

	mappers::LocalizationMapper locs;

	std::stringstream culInput;
	culInput << "link = { eu4 = bakan ck3 = akan }";
	culInput << "link = { eu4 = bakru ck3 = kru }";
	mappers::CultureMapper culs(culInput);

	std::stringstream input;
	input << "cultures={\n";
	input << "\t13={name=\"Akan-Kru\" name_list=name_list_akan name_list=name_list_kru heritage=heritage_test }\n";
	input << "}\n";
	CK3::Cultures cultures(input);
	cultures.concoctCultures(locs, culs);

	culs.storeCultures(cultures.getCultures());

	// We just generated the name from the culture defs through concoction.
	const auto ck3culture = cultures.getCultures().at(13);
	EXPECT_EQ("dynamic-akan-kru-culture-num1", ck3culture->getName());
	EXPECT_TRUE(ck3culture->isDynamic());

	// we now need an operational heritage mapper that will shove the finished culture def into groupB.
	std::stringstream input2;
	input2 << "link = { eu4 = groupB ck3 = heritage_test }";
	const mappers::HeritageMapper heritages(input2);

	// and we need name lists for both Akan and Kru
	// We'll mess it up so akan doesn't have names but its culture group does.
	std::stringstream input3;
	input3 << "groupA = { bakan = {} male_names = { bob } }\n";
	input3 << "groupB = { bakru = { male_names = { jon } } }\n";
	mappers::CultureDefinitionsMapper mapper(input3);
	mapper.loadHeritages(heritages);

	// build the defs
	mapper.buildDefinitions(culs);

	auto groupB = mapper.getCultureGroup("groupB");
	EXPECT_TRUE(groupB->containsCulture("dynamic-akan-kru-culture-num1"));

	// and print it out - it will print out *only* dynamic cultures, ie, not akan or kru.
	std::stringstream output;
	output << "groupB = {\n";
	output << "\tdynamic-akan-kru-culture-num1 = {\n";
	output << "\t\tmale_names = { \"bob\" \"jon\" }\n";
	output << "\t}\n";
	output << "}\n\n";

	std::stringstream actual;
	actual << mapper;

	EXPECT_EQ(output.str(), actual.str());
}
