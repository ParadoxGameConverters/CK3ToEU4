#include "../../CK3ToEU4/Source/Mappers/LocalizationMapper/LocalizationMapper.h"
#include "CommonFunctions.h"
#include "gtest/gtest.h"

TEST(Mappers_LocalisationTests, localisationsCanBeLoadedAndMatched)
{
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " key1: \"value 1\" # comment\n";
	input << " key2: \"value \"subquoted\" 2\"\n";
	std::stringstream input2;
	input2 << commonItems::utf8BOM << "l_french:\n";
	input2 << " key1: \"valuee 1\"\n";
	input2 << " key2: \"valuee \"subquoted\" 2\"\n";
	std::stringstream input3;
	input3 << commonItems::utf8BOM << "l_english:\n";
	input3 << " key1: \"replaced value 1\"\n";

	mappers::LocalizationMapper locs;
	locs.scrapeStream(input, "english");
	locs.scrapeStream(input2, "french");
	locs.scrapeStream(input3, "english");

	EXPECT_EQ("replaced value 1", locs.getLocBlockForKey("key1")->english);
	EXPECT_EQ("value \"subquoted\" 2", locs.getLocBlockForKey("key2")->english);
	EXPECT_EQ("valuee 1", locs.getLocBlockForKey("key1")->french);
	EXPECT_EQ("valuee \"subquoted\" 2", locs.getLocBlockForKey("key2")->french);
}

TEST(Mappers_LocalisationTests, localisationsReturnNulloptForMissingKey)
{
	const mappers::LocalizationMapper locs;

	EXPECT_FALSE(locs.getLocBlockForKey("key1"));
}

TEST(Mappers_LocalisationTests, localisationsReturnsEnglishForMissingLanguage)
{
	mappers::LocalizationMapper locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " key1: \"value 1\" # comment\n";
	locs.scrapeStream(input, "english");

	EXPECT_EQ("value 1", locs.getLocBlockForKey("key1")->french);
}

TEST(Mappers_LocalisationTests, reverseLookupCultureNameWorks)
{
	mappers::LocalizationMapper locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_korean:\n";
	input << " culture_name: \"The Name\"\n";
	locs.scrapeStream(input, "korean");

	EXPECT_EQ("culture_name", locs.reverseLookupCultureName("The Name"));
}

TEST(Mappers_LocalisationTests, reverseLookupCultureNameDoesNotWorkForNonCultures)
{
	mappers::LocalizationMapper locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_korean:\n";
	input << " d_culture_adj: \"The Name\"\n";
	locs.scrapeStream(input, "korean");

	EXPECT_EQ(std::nullopt, locs.reverseLookupCultureName("The Name"));
}

TEST(Mappers_LocalisationTests, nestedLocalisationsCanBeUnravelled)
{
	mappers::LocalizationMapper locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " nested_key: \"This $n$ has nested $locs$\"\n";
	input << " n: \"$noun$\"\n";
	input << " locs: \"localizations\"\n";
	input << " noun: \"string\"\n";

	locs.scrapeStream(input, "english");
	auto copyblock = locs.getLocBlockForKey("nested_key");
	locs.unravelNestedLocs(copyblock.value());
	EXPECT_EQ("This string has nested localizations", copyblock->english);
}

TEST(Mappers_LocalisationTests, stringsCanBeSelectedFor)
{
	std::string a = "I_do_not_fear_the_big_blue_blob.";
	std::string b = "Ich fürchte den großen  blauen Klecks nicht.";
	std::string c = "Je$n'ai$pas$peur$du$gros$blob$bleu.";
	std::string d = "No__le__temo__a__la__gran__mancha__azul.";

	EXPECT_EQ(mappers::getLeadStr(a), "I");
	EXPECT_EQ(mappers::getLeadStr(d), "No");
	EXPECT_EQ(mappers::getTailStr(a), "do_not_fear_the_big_blue_blob.");
	EXPECT_EQ(mappers::getTailStr(d), "_le__temo__a__la__gran__mancha__azul.");

	EXPECT_EQ(mappers::getLeadStr(d, 1, "__"), "No");
	EXPECT_EQ(mappers::getTailStr(d, 1, "__"), "le__temo__a__la__gran__mancha__azul.");

	EXPECT_EQ(mappers::getLeadStr(b, 1, " "), "Ich");
	EXPECT_EQ(mappers::getLeadStr(b, 1, "  "), "Ich fürchte den großen");
	EXPECT_EQ(mappers::getTailStr(b, 1, " "), "fürchte den großen  blauen Klecks nicht.");
	EXPECT_EQ(mappers::getTailStr(b, 1, "  "), "blauen Klecks nicht.");
	EXPECT_EQ(mappers::getTailStr(b, 4, " "), " blauen Klecks nicht.");

	EXPECT_EQ(mappers::getLeadStr(c, 5, "$"), "Je$n'ai$pas$peur$du");
	EXPECT_EQ(mappers::getTailStr(c, 5, "$"), "gros$blob$bleu.");
	EXPECT_EQ(mappers::getLeadStr(d, 6), "No__le__temo_");
	EXPECT_EQ(mappers::getLeadStr(d, 6, "__"), "No__le__temo__a__la__gran");
	EXPECT_EQ(mappers::getTailStr(d, 6), "a__la__gran__mancha__azul.");
	EXPECT_EQ(mappers::getTailStr(d, 6, "__"), "mancha__azul.");
}

TEST(Mappers_LocalisationTests, trivialEmphasisCanBeRemoved)
{
	mappers::LocalizationMapper locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " key: \"random #F Halale, or truth, of halale. and recognition.#!\"\n";

	locs.scrapeStream(input, "english");
	auto copyblock = locs.getLocBlockForKey("key");
	locs.removeEmphasis(*copyblock);
	EXPECT_EQ("random Halale, or truth, of halale. and recognition.", copyblock->english);
}

TEST(Mappers_LocalisationTests, multipleTrivialEmphasisCanBeRemoved)
{
	mappers::LocalizationMapper locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " key: \"random #F Halale, or truth#!, of #EMPHASIS halale. and recognition.#!\"\n";

	locs.scrapeStream(input, "english");
	auto copyblock = locs.getLocBlockForKey("key");
	locs.removeEmphasis(*copyblock);
	EXPECT_EQ("random Halale, or truth, of halale. and recognition.", copyblock->english);
}

TEST(Mappers_LocalisationTests, nestedEmphasisCanBeRemoved)
{
	mappers::LocalizationMapper locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " key: \"#RANDOMTESTMARK #TESTMARK Halale#!, or truth, of #TESTMARK2 halale#!. and recognition.#!\"\n";

	locs.scrapeStream(input, "english");
	auto copyblock = locs.getLocBlockForKey("key");
	locs.removeEmphasis(*copyblock);
	EXPECT_EQ("Halale, or truth, of halale. and recognition.", copyblock->english);
}

TEST(Mappers_LocalisationTests, complexEmphasisCanBeRemoved)
{
	mappers::LocalizationMapper locs;
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " key: \"#F #italic Halale#!, or truth, of #italic halale#!. and recognition.#!\"\n";

	locs.scrapeStream(input, "english");
	auto copyblock = locs.getLocBlockForKey("key");
	locs.removeEmphasis(*copyblock);
	EXPECT_EQ("Halale, or truth, of halale. and recognition.", copyblock->english);
}
