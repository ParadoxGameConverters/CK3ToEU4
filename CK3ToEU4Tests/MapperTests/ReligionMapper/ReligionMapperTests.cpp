#include "../../CK3ToEU4/Source/CK3World/Religions/Faiths.h"
#include "../../CK3ToEU4/Source/Mappers/LocalizationMapper/LocalizationMapper.h"
#include "../../CK3ToEU4/Source/Mappers/ReligionDefinitionMapper/ReligionDefinitionMapper.h"
#include "../../CK3ToEU4/Source/Mappers/ReligionGroupScraper/ReligionGroupScraper.h"
#include "../../CK3ToEU4/Source/Mappers/ReligionMapper/ReligionMapper.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(Mappers_ReligionMapperTests, nonMatchGivesEmptyOptional)
{
	std::stringstream input;
	input << "link = { eu4 = eu4Religion ck3 = ck3Religion }";

	const mappers::ReligionMapper theMapper(input);

	const auto& eu4Religion = theMapper.getEU4ReligionForCK3Religion("nonMatchingReligion", "");
	ASSERT_FALSE(eu4Religion);
}

TEST(Mappers_ReligionMapperTests, eu4ReligionCanBeFound)
{
	std::stringstream input;
	input << "link = { eu4 = eu4Religion ck3 = ck3Religion }";

	const mappers::ReligionMapper theMapper(input);

	const auto& eu4Religion = theMapper.getEU4ReligionForCK3Religion("ck3Religion", "");
	ASSERT_EQ("eu4Religion", eu4Religion);
}

TEST(Mappers_ReligionMapperTests, eu4SchoolCanBeFound)
{
	std::stringstream input;
	input << "link = { eu4 = eu4Religion ck3 = ck3Religion school = some_school }";

	const mappers::ReligionMapper theMapper(input);

	const auto& eu4School = theMapper.getEU4SchoolForCK3Religion("ck3Religion");
	ASSERT_EQ("some_school", *eu4School);
}


TEST(Mappers_ReligionMapperTests, multipleEU4ReligionsCanBeInARule)
{
	std::stringstream input;
	input << "link = { eu4 = eu4Religion ck3 = ck3Religion ck3 = ck3Religion2 }";

	const mappers::ReligionMapper theMapper(input);

	const auto& eu4Religion = theMapper.getEU4ReligionForCK3Religion("ck3Religion2", "");
	ASSERT_EQ("eu4Religion", eu4Religion);
}


TEST(Mappers_ReligionMapperTests, correctRuleMatches)
{
	std::stringstream input;
	input << "link = { eu4 = eu4Religion ck3 = ck3Religion }";
	input << "link = { eu4 = eu4Religion2 ck3 = ck3Religion2 }";

	const mappers::ReligionMapper theMapper(input);

	const auto& eu4Religion = theMapper.getEU4ReligionForCK3Religion("ck3Religion2", "");
	ASSERT_EQ("eu4Religion2", eu4Religion);
}

TEST(Mappers_ReligionMapperTests, faithCanBeImported)
{
	std::stringstream input;
	input << "link = { eu4 = religion ck3 = faith1 }\n";
	input << "link = { eu4 = religion2 ck3 = faith2 ck3 = faith3 }\n"; // <- we'll use this one.
	input << "link = { eu4 = religion3 ck3 = faith4 }\n";
	mappers::ReligionMapper theMapper(input);

	std::stringstream input2;
	input2 << "13={tag=dyn_faith_345\n"; // dynamic faith
	input2 << "template=faith2\n";		 // <- this is the link
	input2 << "icon=someicon.dds\n";
	input2 << "color=rgb { 229 178 76 }\n";
	input2 << "name = \"Custom Name\"\n";
	input2 << "adjective = \"Custom Adj\"\n";
	input2 << "desc = \"Custom Description\"\n";
	input2 << "doctrine=\"tenet_1\"\n";
	input2 << "doctrine=\"tenet_2\"\n";
	input2 << "}";
	const CK3::Faiths faiths(input2);

	std::stringstream input3;
	input3 << "religion_group1 = { religion={} religion2={} }\n"; // <- We're here
	input3 << "religion_group2 = { religion3={} }\n";
	const mappers::ReligionGroupScraper scraper(input3);

	std::stringstream input4;
	input4 << "current_icons = 9\n";
	// Unique ill be overwritten
	input4 << "tenet_1 = { country={country=great} province={my provinces are golden} unique_modifiers={I Am The Pope}}\n";
	// Only the last unique will apply.
	input4 << "tenet_2 = { country={country=poor} province={my provinces are barren} unique_modifiers={I Am Not The Pope}}\n";
	// Irrelevant to us
	input4 << "tenet_3 = { country={country=whatcountry} province={my provinces are stolen} unique_modifiers={I Use Karma}}\n";
	std::stringstream input5;
	input5 << "religion = {Static Template, usually very long.}\n";
	input5 << "religion2 = {Static Template, usually very short.}\n"; // <- this applies
	mappers::ReligionDefinitionMapper definitions(input4, input5);

	mappers::LocalizationMapper localizationMapper;

	ASSERT_FALSE(theMapper.getEU4ReligionForCK3Religion("dyn_faith_345", "")); // There is no mapping like this, yet.
	theMapper.importCK3Faiths(faiths, definitions, scraper, localizationMapper);
	ASSERT_TRUE(theMapper.getEU4ReligionForCK3Religion("dyn_faith_345", "")); // Now something exists.

	const auto& match = theMapper.getEU4ReligionForCK3Religion("dyn_faith_345", "");
	ASSERT_EQ("converted_dyn_faith_345", *match); // it's a new mapping we created.

	const auto& dynamics = theMapper.getGeneratedReligions();
	const auto newReligion = dynamics[0]; // these will be its details.

	ASSERT_EQ("country=great\ncountry=poor\n", newReligion.country);
	ASSERT_TRUE(newReligion.countrySecondary.empty());
	ASSERT_EQ("my provinces are golden\nmy provinces are barren\n", newReligion.province);
	ASSERT_EQ("I Am Not The Pope", newReligion.unique);
	ASSERT_EQ("Static Template, usually very short.", newReligion.staticBlob);
	ASSERT_TRUE(newReligion.nonUnique.empty());
	ASSERT_EQ(10, newReligion.icon);
	ASSERT_EQ("someicon.dds", newReligion.iconPath);
	ASSERT_EQ("religion_group1", newReligion.religionGroup);
	ASSERT_EQ("converted_dyn_faith_345", newReligion.name);
	ASSERT_EQ("religion2", newReligion.parent);

	const auto& locs = theMapper.getLocalizations();
	const auto& newLocs = locs.find("converted_dyn_faith_345");

	ASSERT_EQ("Custom Adj", newLocs->second.english);
	ASSERT_EQ("Custom Adj", newLocs->second.spanish);
	ASSERT_EQ("Custom Adj", newLocs->second.german);
	ASSERT_EQ("Custom Adj", newLocs->second.french);

	const auto& descLocs = locs.find("converted_dyn_faith_345_religion_desc");

	ASSERT_EQ("Custom Description", descLocs->second.english);
	ASSERT_EQ("Custom Description", descLocs->second.spanish);
	ASSERT_EQ("Custom Description", descLocs->second.german);
	ASSERT_EQ("Custom Description", descLocs->second.french);
}