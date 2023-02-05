#include "../../CK3ToEU4/Source/Mappers/CultureMapper/CultureMapper.h"
#include "../CK3ToEU4/Source/CK3World/Cultures/Culture.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

#include "CommonFunctions.h"
using testing::ElementsAre;
TEST(CK3World_CultureTests, cultureIDLoads)
{
	std::stringstream input;
	const CK3::Culture culture(input, 42);

	EXPECT_EQ(42, culture.getID());
}

TEST(CK3World_CultureTests, loadValuesDefaultToDefaults)
{
	std::stringstream input;
	const CK3::Culture culture(input, 1);

	EXPECT_TRUE(culture.getName().empty());
	EXPECT_FALSE(culture.isDynamic());
	EXPECT_FALSE(culture.isEU4Ready());
	EXPECT_TRUE(culture.getHeritage().empty());
	EXPECT_TRUE(culture.getEthos().empty());
	EXPECT_EQ(std::nullopt, culture.getLocalizedName());
	EXPECT_TRUE(culture.getName().empty());
	EXPECT_EQ(std::nullopt, culture.getTemplate());
	EXPECT_TRUE(culture.getNameLists().empty());
	EXPECT_TRUE(culture.getTraditions().empty());
}

TEST(CK3World_CultureTests, culturePrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "culture_template = akan\n";
	input << "name = \"Akan\"\n";
	input << "heritage = heritage_akan\n";
	input << "name_list = name_list_akan\n";
	input << "name_list = name_list_blahakan\n";
	input << "ethos = ethos_egalitarian\n";
	input << "traditions = { tradition_parochialism tradition_bush_hunting }\n";

	const CK3::Culture culture(input, 42);

	EXPECT_EQ("akan", culture.getTemplate());
	EXPECT_EQ("Akan", culture.getLocalizedName());
	EXPECT_EQ("heritage_akan", culture.getHeritage());
	EXPECT_EQ("akan", culture.getTemplate());
	EXPECT_EQ("ethos_egalitarian", culture.getEthos());
	EXPECT_THAT(culture.getTraditions(), ElementsAre("tradition_parochialism", "tradition_bush_hunting"));
	EXPECT_THAT(culture.getNameLists(), ElementsAre("akan", "blahakan"));
	EXPECT_FALSE(culture.isDynamic());
	EXPECT_FALSE(culture.isEU4Ready());
	EXPECT_TRUE(culture.getName().empty());
}

TEST(CK3World_CultureTests, concoctingTemplateGeneratesName)
{
	mappers::LocalizationMapper locs;
	mappers::CultureMapper culs;
	std::map<std::string, int> cultureCounter;

	std::stringstream input;
	input << "culture_template=akan";
	CK3::Culture culture(input, 42);

	culture.concoctCultureName(locs, culs, cultureCounter);

	EXPECT_EQ("akan", culture.getName());
	// neither dynamic nor eu4ready.
	EXPECT_FALSE(culture.isDynamic());
	EXPECT_FALSE(culture.isEU4Ready());
}

TEST(CK3World_CultureTests, concoctingBlanknessGeneratesNoname)
{
	mappers::LocalizationMapper locs;
	mappers::CultureMapper culs;
	std::map<std::string, int> cultureCounter;

	std::stringstream input;
	CK3::Culture culture(input, 42);

	culture.concoctCultureName(locs, culs, cultureCounter);

	EXPECT_EQ("noname", culture.getName());
	// neither dynamic nor eu4ready.
	EXPECT_FALSE(culture.isDynamic());
	EXPECT_FALSE(culture.isEU4Ready());
}

TEST(CK3World_CultureTests, concoctingViaLocalizationIntoEU4Ready)
{
	// MEAT 1: If a culture is reverse localized into something we have mapped already *into* - then keep it!

	mappers::LocalizationMapper locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " austrian_name: \"Austrian\"\n";
	locs.scrapeStream(input, "english");

	std::stringstream input2;
	input2 << "link = { eu4 = austrian ck3 = irrelevant }";
	const mappers::CultureMapper culs(input2);

	std::map<std::string, int> cultureCounter;

	std::stringstream input3;
	input3 << "name = \"Austrian\"";
	CK3::Culture culture(input3, 42);

	culture.concoctCultureName(locs, culs, cultureCounter);

	EXPECT_EQ("austrian", culture.getName());
	EXPECT_EQ("Austrian", culture.getLocalizedName());
	// not dynamic (which is a lie) but eu4ready.
	EXPECT_FALSE(culture.isDynamic());
	EXPECT_TRUE(culture.isEU4Ready());
}

TEST(CK3World_CultureTests, concoctingViaLocalizationIntoMappable)
{
	// MEAT 2: If a culture is reverse localized into something can map *from* - then keep it!

	mappers::LocalizationMapper locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " austrian_name: \"Austrian\"\n";
	locs.scrapeStream(input, "english");

	std::stringstream input2;
	input2 << "link = { eu4 = irrelevant ck3 = austrian }";
	const mappers::CultureMapper culs(input2);

	std::map<std::string, int> cultureCounter;

	std::stringstream input3;
	input3 << "name = \"Austrian\"";
	CK3::Culture culture(input3, 42);

	culture.concoctCultureName(locs, culs, cultureCounter);

	EXPECT_EQ("austrian", culture.getName());
	EXPECT_EQ("Austrian", culture.getLocalizedName());
	// not dynamic (which is a lie) and not eu4ready.
	EXPECT_FALSE(culture.isDynamic());
	EXPECT_FALSE(culture.isEU4Ready());
}

TEST(CK3World_CultureTests, concoctingDefaultsToCK3NamesWithoutMappings)
{
	// MEAT 3: For anything unrecognizable, generate a name. with failed Mappings, use CK3 names.

	const mappers::LocalizationMapper locs;
	const mappers::CultureMapper culs;

	std::map<std::string, int> cultureCounter;

	std::stringstream input;
	input << "name = \"Austrian\"";
	input << "name_list = name_list_akan\n";
	input << "name_list = name_list_blahakan\n";
	CK3::Culture culture(input, 42);

	culture.concoctCultureName(locs, culs, cultureCounter);

	EXPECT_EQ("dynamic-akan-blahakan-culture-num1", culture.getName());
	EXPECT_EQ("Austrian", culture.getLocalizedName());
	// dynamic and not eu4ready.
	EXPECT_TRUE(culture.isDynamic());
	EXPECT_FALSE(culture.isEU4Ready());
}

TEST(CK3World_CultureTests, concoctingUsesEU4NamesWithMappings)
{
	// MEAT 3: For anything unrecognizable, generate a name. With proper mappings, use EU4 names.

	const mappers::LocalizationMapper locs;

	std::stringstream culInput;
	culInput << "link = { eu4 = test1 ck3 = akan }";
	culInput << "link = { eu4 = test2 ck3 = blahakan }";
	const mappers::CultureMapper culs(culInput);

	std::map<std::string, int> cultureCounter;

	std::stringstream input;
	input << "name = \"Austrian\"";
	input << "name_list = name_list_akan\n";
	input << "name_list = name_list_blahakan\n";
	CK3::Culture culture(input, 42);

	culture.concoctCultureName(locs, culs, cultureCounter);

	EXPECT_EQ("dynamic-test1-test2-culture-num1", culture.getName());
	EXPECT_EQ("Austrian", culture.getLocalizedName());
	// dynamic and not eu4ready.
	EXPECT_TRUE(culture.isDynamic());
	EXPECT_FALSE(culture.isEU4Ready());
}

TEST(CK3World_CultureTests, concoctingSeenDefaultsIncreasesNumbers)
{
	// MEAT 3b: If you have seen this combo elsewhere, make a new name.

	const mappers::LocalizationMapper locs;
	const mappers::CultureMapper culs;

	std::map<std::string, int> cultureCounter;
	cultureCounter.emplace("dynamic-akan-blahakan-culture", 4);

	std::stringstream input;
	input << "name = \"Austrian\"";
	input << "name_list = name_list_akan\n";
	input << "name_list = name_list_blahakan\n";
	CK3::Culture culture(input, 42);

	culture.concoctCultureName(locs, culs, cultureCounter);

	EXPECT_EQ("dynamic-akan-blahakan-culture-num5", culture.getName());
	EXPECT_EQ("Austrian", culture.getLocalizedName());
	// dynamic and not eu4ready.
	EXPECT_TRUE(culture.isDynamic());
	EXPECT_FALSE(culture.isEU4Ready());
}
