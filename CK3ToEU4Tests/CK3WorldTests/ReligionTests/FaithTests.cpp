#include "../CK3toEU4/Source/CK3World/Religions/Faith.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_FaithTests, faithIDLoads)
{
	std::stringstream input;
	const CK3::Faith faith(input, 42);

	ASSERT_EQ(42, faith.getID());
}

TEST(CK3World_FaithTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::Faith faith(input, 1);

	ASSERT_TRUE(faith.getName().empty());
	ASSERT_TRUE(faith.getDoctrines().empty());
	ASSERT_FALSE(faith.getColor());
	ASSERT_FALSE(faith.getReligion().first);
	ASSERT_TRUE(faith.getCustomName().empty());
	ASSERT_TRUE(faith.getCustomAdj().empty());
	ASSERT_TRUE(faith.getDescription().empty());
	ASSERT_TRUE(faith.getTemplate().empty());
	ASSERT_TRUE(faith.getIconPath().empty());
}

TEST(CK3World_FaithTests, faithPrimitivesCanBeLoaded)
{
	std::stringstream input;
	input << "tag=\"akom_pagan\"\n";
	input << "color=rgb { 229 178 76 }icon=\"i\\c\\on_path.dds\"\n";
	input << "doctrine=\"tenet_adorcism\"\n";
	input << "doctrine=\"doctrine_monogamy\"\n";
	input << "doctrine=\"doctrine_deviancy_shunned\"\n";
	input << "religion = 7\n";
	input << "name = \"Custom Name\"\n";
	input << "adjective = \"Custom Adj\"\n";
	input << "desc = \"Custom Desc\"\n";
	input << "icon = \"gfx/icon.dds\"\n";
	input << "template = \"catholic\"\n";

	const CK3::Faith faith(input, 42);

	ASSERT_EQ("akom_pagan", faith.getName());
	ASSERT_EQ(commonItems::Color(std::array<int, 3>{229, 178, 76}), faith.getColor());
	ASSERT_EQ(3, faith.getDoctrines().size());
	ASSERT_EQ(1, faith.getDoctrines().count("tenet_adorcism"));
	ASSERT_EQ(1, faith.getDoctrines().count("doctrine_monogamy"));
	ASSERT_EQ(1, faith.getDoctrines().count("doctrine_deviancy_shunned"));
	ASSERT_EQ(7, faith.getReligion().first);
	ASSERT_EQ("Custom Name", faith.getCustomName());
	ASSERT_EQ("Custom Adj", faith.getCustomAdj());
	ASSERT_EQ("Custom Desc", faith.getDescription());
	ASSERT_EQ("gfx/icon.dds", faith.getIconPath());
	ASSERT_EQ("catholic", faith.getTemplate());
}
