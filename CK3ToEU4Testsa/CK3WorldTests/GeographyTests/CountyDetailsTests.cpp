#include "../../CK3ToEU4/Source/CK3World/Cultures/Culture.h"
#include "../../CK3ToEU4/Source/CK3World/Cultures/Cultures.h"
#include "../../CK3ToEU4/Source/CK3World/Geography/CountyDetail.h"
#include "../../CK3ToEU4/Source/CK3World/Geography/CountyDetails.h"
#include "../../CK3ToEU4/Source/CK3World/Religions/Faith.h"
#include "../../CK3ToEU4/Source/CK3World/Religions/Faiths.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_CountyDetailsTests, detailsDefaultToEmpty)
{
	std::stringstream input;
	const CK3::CountyDetails details(input);

	ASSERT_TRUE(details.getCountyDetails().empty());
}

TEST(CK3World_CountyDetailsTests, detailsCanBeLoadedIfPresent)
{
	std::stringstream input;
	input << "c_county1 = {}\n";
	input << "c_county2 = { development = 8 }\n";
	input << "c_county3 = { faith = 4 }\n";

	const CK3::CountyDetails details(input);
	const auto& c1 = details.getCountyDetails().find("c_county1");
	const auto& c2 = details.getCountyDetails().find("c_county2");
	const auto& c3 = details.getCountyDetails().find("c_county3");

	ASSERT_EQ(3, details.getCountyDetails().size());
	ASSERT_FALSE(c1->second->getDevelopment());
	ASSERT_EQ(8, c2->second->getDevelopment());
	ASSERT_EQ(4, c3->second->getFaith().first);
}

TEST(CK3World_CountyDetailsTests, culturesCanBeLinked)
{
	std::stringstream input;
	input << "13={culture_template=\"akan\"}\n";
	input << "15={culture_template=\"kru\"}\n";
	const CK3::Cultures cultures(input);

	std::stringstream input2;
	input2 << "c_county1 = { culture = 15 }\n";
	input2 << "c_county2 = { culture = 13 }\n";
	CK3::CountyDetails details(input2);
	details.linkCultures(cultures);

	const auto& c1 = details.getCountyDetails().find("c_county1");
	const auto& c2 = details.getCountyDetails().find("c_county2");

	ASSERT_EQ("kru", c1->second->getCulture().second->getTemplate());
	ASSERT_EQ("akan", c2->second->getCulture().second->getTemplate());
}

TEST(CK3World_CountyDetailsTests, linkingMissingCultureThrowsException)
{
	std::stringstream input;
	input << "13={culture_template=\"akan\"}\n";
	const CK3::Cultures cultures(input);

	std::stringstream input2;
	input2 << "c_county2 = { culture = 13 }\n";
	input2 << "c_county1 = { culture = 15 }\n";
	CK3::CountyDetails details(input2);

	ASSERT_THROW(details.linkCultures(cultures), std::runtime_error);
}

TEST(CK3World_CountyDetailsTests, faithsCanBeLinked)
{
	std::stringstream input;
	input << "13={tag=\"old_bon\"}\n";
	input << "15={tag=\"theravada\"}\n";
	const CK3::Faiths faiths(input);

	std::stringstream input2;
	input2 << "c_county1 = { faith = 15 }\n";
	input2 << "c_county2 = { faith = 13 }\n";
	CK3::CountyDetails details(input2);
	details.linkFaiths(faiths);

	const auto& c1 = details.getCountyDetails().find("c_county1");
	const auto& c2 = details.getCountyDetails().find("c_county2");

	ASSERT_EQ("theravada", c1->second->getFaith().second->getName());
	ASSERT_EQ("old_bon", c2->second->getFaith().second->getName());
}

TEST(CK3World_CountyDetailsTests, linkingMissingFaithThrowsException)
{
	std::stringstream input;
	input << "13={tag=\"old_bon\"}\n";
	const CK3::Faiths faiths(input);

	std::stringstream input2;
	input2 << "c_county1 = { faith = 13 }\n";
	input2 << "c_county2 = { faith = 15 }\n";
	CK3::CountyDetails details(input2);

	ASSERT_THROW(details.linkFaiths(faiths), std::runtime_error);
}
