#include "../../CK3ToEU4/Source/CK3World/Confederations/Confederation.h"
#include "../../CK3ToEU4/Source/CK3World/Confederations/Confederations.h"
#include "gtest/gtest.h"

TEST(CK3World_ConfederationsTests, ConfederationsDefaultToEmpty)
{
	std::stringstream input;
	const CK3::Confederations confederations(input);

	EXPECT_TRUE(confederations.getConfederations().empty());
}

TEST(CK3World_ConfederationsTests, ConfederationsCanBeLoaded)
{
	std::stringstream input;

	input << "database={\n";
	input << "		0={\n";
	input << "			members={ 16787777 9525 9944 33566183 16795849 16818520 }\n";
	input << "			name=\"Irish Confederation\"\n";
	input << "			color=rgb { 49 249 72 }\n";
	input << "			coat_of_arms=17763\n";
	input << "			variables={\n";
	input << "				data={ {\n";
	input << "						flag=confederation_culture\n";
	input << "						data={\n";
	input << "							type=culture\n";
	input << "							identity=65\n";
	input << "						}\n";
	input << "					}\n";
	input << "}\n";
	input << "			}\n";
	input << "		}\n";
	input << "		16777217={\n";
	input << "			members={ 39303 26333 33589596 10062 40945 16807097 46100 50346833 }\n";
	input << "			name=\"Khazar Confederation\"\n";
	input << "			color=rgb { 210 160 69 }\n";
	input << "			coat_of_arms=31944\n";
	input << "			variables={\n";
	input << "				data={ {\n";
	input << "						flag=confederation_culture\n";
	input << "						data={\n";
	input << "							type=culture\n";
	input << "							identity=157\n";
	input << "						}\n";
	input << "					}\n";
	input << " }\n";
	input << "			}\n";
	input << "}\n";
	input << "		16777218=none\n";
	input << "}\n";

	const CK3::Confederations confederations(input);
	const auto& c1 = confederations.getConfederations().find(0);
	const auto& c2 = confederations.getConfederations().find(16777217);

	EXPECT_EQ(2, confederations.getConfederations().size());
	EXPECT_EQ("Irish Confederation", c1->second->getName());
	EXPECT_EQ("Khazar Confederation", c2->second->getName());
}
