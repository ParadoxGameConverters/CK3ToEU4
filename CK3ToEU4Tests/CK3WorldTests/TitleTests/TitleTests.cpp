#include "../CK3toEU4/Source/CK3World/Characters/Character.h"
#include "../CK3toEU4/Source/CK3World/Characters/Characters.h"
#include "../CK3toEU4/Source/CK3World/Geography/CountyDetail.h"
#include "../CK3toEU4/Source/CK3World/Geography/ProvinceHolding.h"
#include "../CK3toEU4/Source/CK3World/Titles/LandedTitles.h"
#include "../CK3toEU4/Source/CK3World/Titles/Title.h"
#include "../CK3toEU4/Source/CK3World/Titles/Titles.h"
#include "../CK3toEU4/Source/Mappers/DevWeightsMapper/DevWeightsMapper.h"
#include "gtest/gtest.h"

TEST(CK3World_TitleTests, idCanBeSet)
{
	std::stringstream input;
	const CK3::Title theTitle(input, 1);

	ASSERT_EQ(1, theTitle.getID());
}

TEST(CK3World_TitleTests, loadValuesDefaultToBlank)
{
	std::stringstream input;
	const CK3::Title theTitle(input, 1);

	ASSERT_TRUE(theTitle.getName().empty());
	ASSERT_TRUE(theTitle.getDisplayName().empty());
	ASSERT_TRUE(theTitle.getAdjective().empty());
	ASSERT_TRUE(theTitle.getHistoryGovernment().empty());
	ASSERT_TRUE(theTitle.getLaws().empty());
	ASSERT_FALSE(theTitle.getHolder());
	ASSERT_FALSE(theTitle.isTheocraticLease());
	ASSERT_FALSE(theTitle.isCountyCapitalBarony());
	ASSERT_FALSE(theTitle.isDuchyCapitalBarony());
	ASSERT_FALSE(theTitle.getCapital().first);
	ASSERT_FALSE(theTitle.getDFLiege());
	ASSERT_FALSE(theTitle.getDJLiege());
	ASSERT_TRUE(theTitle.getDJVassals().empty());
	ASSERT_TRUE(theTitle.getHeirs().empty());
	ASSERT_TRUE(theTitle.getClaimants().empty());
	ASSERT_TRUE(theTitle.getElectors().empty());
	ASSERT_FALSE(theTitle.getCoA());
	ASSERT_FALSE(theTitle.getColor());
	ASSERT_FALSE(theTitle.isLandless());
}

TEST(CK3World_TitleTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "key=\"k_grenada\"\n";
	input << "name=\"Cuman Empire\"\n";
	input << "adj=\"the \"\n";
	input << "history_government=\"theocratic_government\"\n";
	input << "laws={ law_1 second_law second_law second_law law_succession_3 }\n";
	input << "holder=123\n";
	input << "theocratic_lease=yes\n";
	input << "capital_barony=yes\n";
	input << "duchy_capital_barony=yes\n";
	input << "capital=123\n";
	input << "de_facto_liege=1234\n";
	input << "de_jure_liege=12345\n";
	input << "de_jure_vassals={ 1 2 3 4 5 }\n";
	input << "heir={ 3 4 5 }\n";
	input << "claim={ 7 8 }\n";
	input << "coat_of_arms_id=45\n";
	input << "color={ 1 2 3 }\n";
	input << "landless=yes\n";

	const CK3::Title theTitle(input, 1);

	ASSERT_EQ("k_grenada", theTitle.getName());
	ASSERT_EQ("Cuman Empire", theTitle.getDisplayName());
	ASSERT_EQ("the ", theTitle.getAdjective());
	ASSERT_EQ("theocratic_government", theTitle.getHistoryGovernment());
	ASSERT_EQ(3, theTitle.getLaws().size());
	ASSERT_EQ(1, theTitle.getLaws().count("second_law"));
	ASSERT_EQ(123, theTitle.getHolder()->first);
	ASSERT_TRUE(theTitle.isTheocraticLease());
	ASSERT_TRUE(theTitle.isCountyCapitalBarony());
	ASSERT_TRUE(theTitle.isDuchyCapitalBarony());
	ASSERT_EQ(123, theTitle.getCapital().first);
	ASSERT_EQ(1234, theTitle.getDFLiege()->first);
	ASSERT_EQ(12345, theTitle.getDJLiege()->first);
	ASSERT_EQ(5, theTitle.getDJVassals().size());
	ASSERT_EQ(1, theTitle.getDJVassals().count(1));
	ASSERT_EQ(3, theTitle.getHeirs().size());
	ASSERT_EQ(3, theTitle.getHeirs()[0].first);
	ASSERT_EQ(4, theTitle.getHeirs()[1].first);
	ASSERT_EQ(5, theTitle.getHeirs()[2].first);
	ASSERT_EQ(2, theTitle.getClaimants().size());
	ASSERT_EQ(1, theTitle.getClaimants().count(7));
	ASSERT_EQ(1, theTitle.getClaimants().count(8));
	ASSERT_EQ(45, theTitle.getCoA()->first);
	ASSERT_EQ(commonItems::Color(std::array<int, 3>{1, 2, 3}), theTitle.getColor());
	ASSERT_TRUE(theTitle.isLandless());
}

TEST(CK3World_TitleTests, nameCanBeCleanedOfGUIJunk)
{
	std::stringstream input;
	input << "key=\"k_grenada\"\n";
	input << "name=\"Crusader \x15ONCLICK:TITLE,81 \x15TOOLTIP:LANDED_TITLE,81 \x15L England\x15!\x15!\x15!\"\n";
	const CK3::Title theTitle(input, 1);
	ASSERT_EQ("Crusader England", theTitle.getDisplayName());

	std::stringstream input2;
	input2 << "key=\"k_grenada\"\n";
	input2
		 << "name=\"\x15TOOLTIP:FAITH,catholic \x15L Catholic\x15!\x15! \x15ONCLICK:TITLE,733 \x15TOOLTIP:LANDED_TITLE,733 \x15L Lotharingia\x15!\x15!\x15!\"\n";
	const CK3::Title theTitle2(input2, 1);
	ASSERT_EQ("Catholic Lotharingia", theTitle2.getDisplayName());
}


TEST(CK3World_TitleTests, titleWithoutHolderResetsDFLiege) // this is done because this title is in fact dead and savegame is wrong.
{
	std::stringstream input;
	input << "key=\"k_grenada\"\n";
	input << "de_facto_liege=1234\n";
	const CK3::Title theTitle(input, 1);

	ASSERT_FALSE(theTitle.getDFLiege());
}

TEST(CK3World_TitleTests, successionElectionCanBeLoaded)
{
	std::stringstream input;
	input << "succession_election={\n";
	input << "\telectors={ 17972 43082 16968 }\n";
	input << "}\n";

	const CK3::Title theTitle(input, 1);

	ASSERT_EQ(3, theTitle.getElectors().size());
	ASSERT_EQ(17972, theTitle.getElectors().find(17972)->first);
	ASSERT_EQ(43082, theTitle.getElectors().find(43082)->first);
	ASSERT_EQ(16968, theTitle.getElectors().find(16968)->first);
}

TEST(CK3World_TitleTests, previousHoldersCanBeLoaded)
{
	std::stringstream input;
	input << "history={\n";
	input << "\t1.2.3=17972\n";
	input << "\t4.5.6=43082\n";
	input << "\t7.8.9=16968\n";
	input << "}\n";

	const CK3::Title theTitle(input, 1);

	ASSERT_EQ(3, theTitle.getPreviousHolders().size());
	ASSERT_EQ(17972, theTitle.getPreviousHolders()[0].first);
	ASSERT_EQ(43082, theTitle.getPreviousHolders()[1].first);
	ASSERT_EQ(16968, theTitle.getPreviousHolders()[2].first);
}

TEST(CK3World_TitleTests, previousHoldersIgnoresComplexNonsense)
{
	std::stringstream input;
	input << "history={\n";
	input << "\t1.2.3=17972\n";
	input << "\t4.5.6={\n";
	input << "\t\ttype = destroyed\n";
	input << "\n}\n";
	input << "\t7.8.9=16968\n";
	input << "}\n";

	const CK3::Title theTitle(input, 1);

	ASSERT_EQ(2, theTitle.getPreviousHolders().size());
	ASSERT_EQ(17972, theTitle.getPreviousHolders()[0].first);
	ASSERT_EQ(16968, theTitle.getPreviousHolders()[1].first);
}

TEST(CK3World_TitleTests, buildingWeightIsCalculated)
{
	std::stringstream devWeightMapperStream;
	devWeightMapperStream << "dev_from_building = 0.1\n";
	devWeightMapperStream << "dev_from_holding = 0.2\n";
	devWeightMapperStream << "dev_from_development = 0.5\n";
	auto devWeightMapper = mappers::DevWeightsMapper(devWeightMapperStream);

	std::stringstream landedStream;
	landedStream << "b_barony1 = { province = 12 } \n";
	landedStream << "b_barony2 = { province = 13 } \n";
	landedStream << "b_barony3 = { province = 14 } \n";
	landedStream << "c_county = { }\n";
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedStream);
	auto lb1 = landedTitles.getFoundTitles().find("b_barony1")->second;
	auto lb2 = landedTitles.getFoundTitles().find("b_barony2")->second;
	auto lb3 = landedTitles.getFoundTitles().find("b_barony3")->second;
	auto lc = landedTitles.getFoundTitles().find("c_county")->second;

	std::stringstream countyDetailStream;
	countyDetailStream << "development = 7\n";
	auto countyDetail = std::pair("c_county", std::make_shared<CK3::CountyDetail>(countyDetailStream));

	std::stringstream provinceHolding1Stream;
	provinceHolding1Stream << "";
	auto pb1 = std::pair(12, std::make_shared<CK3::ProvinceHolding>(provinceHolding1Stream));
	std::stringstream provinceHolding2Stream;
	provinceHolding2Stream << "holding={type=\"city_holding\"\n}\n";
	auto pb2 = std::pair(13, std::make_shared<CK3::ProvinceHolding>(provinceHolding2Stream));
	std::stringstream provinceHolding3Stream;
	provinceHolding3Stream << "holding={type=\"castle_holding\"\nbuildings={ {type=\"hunting_grounds_01\"} {} {} {type=\"hill_farms_02\"} {} {}}\n";
	auto pb3 = std::pair(14, std::make_shared<CK3::ProvinceHolding>(provinceHolding3Stream));

	std::stringstream titleB1Stream;
	titleB1Stream << "key=b_barony1\n";
	std::pair<long long, std::shared_ptr<CK3::Title>> b1 = std::make_pair(1, std::make_shared<CK3::Title>(titleB1Stream, 1));
	std::stringstream titleB2Stream;
	titleB2Stream << "key=b_barony2\n";
	std::pair<long long, std::shared_ptr<CK3::Title>> b2 = std::make_pair(2, std::make_shared<CK3::Title>(titleB2Stream, 2));
	std::stringstream titleB3Stream;
	titleB3Stream << "key=b_barony3\n";
	std::pair<long long, std::shared_ptr<CK3::Title>> b3 = std::make_pair(3, std::make_shared<CK3::Title>(titleB3Stream, 3));
	std::stringstream titleCStream;
	titleCStream << "key=c_county\nde_jure_vassals={1 2 3}\n";
	std::pair<long long, std::shared_ptr<CK3::Title>> c = std::make_pair(4, std::make_shared<CK3::Title>(titleCStream, 4));

	// Crosslink them.
	lb1->loadProvinceHolding(pb1);
	lb2->loadProvinceHolding(pb2);
	lb3->loadProvinceHolding(pb3);
	lc->loadCountyDetails(countyDetail);
	c.second->loadClay(lc);
	b1.second->loadClay(lb1);
	b2.second->loadClay(lb2);
	b3.second->loadClay(lb3);
	c.second->loadDJVassals(std::map{b1, b2, b3});

	// dev: 7 * 0.5 = 3.5
	// holdings: 2 * 0.2 = 0.4
	// buildings: 2 * 0.1 = 0.2
	// total: 4.1

	ASSERT_NEAR(4.1, c.second->getBuildingWeight(devWeightMapper), 0.001);
}

TEST(CK3World_TitleTests, buildingWeighWithoutCountyfulClayThrowsExceptions)
{
	std::stringstream devWeightMapperStream;
	devWeightMapperStream << "dev_from_building = 0.1\n";
	devWeightMapperStream << "dev_from_holding = 0.2\n";
	devWeightMapperStream << "dev_from_development = 0.5\n";
	auto devWeightMapper = mappers::DevWeightsMapper(devWeightMapperStream);

	std::stringstream titleB1Stream;
	titleB1Stream << "key=b_barony1\n";
	std::pair<long long, std::shared_ptr<CK3::Title>> b1 = std::make_pair(1, std::make_shared<CK3::Title>(titleB1Stream, 1));
	std::stringstream titleB2Stream;
	titleB2Stream << "key=b_barony2\n";
	std::pair<long long, std::shared_ptr<CK3::Title>> b2 = std::make_pair(2, std::make_shared<CK3::Title>(titleB2Stream, 2));
	std::stringstream titleB3Stream;
	titleB3Stream << "key=b_barony3\n";
	std::pair<long long, std::shared_ptr<CK3::Title>> b3 = std::make_pair(3, std::make_shared<CK3::Title>(titleB3Stream, 3));
	std::stringstream titleCStream;
	titleCStream << "key=c_county\nde_jure_vassals={1 2 3}\n";
	std::pair<long long, std::shared_ptr<CK3::Title>> c = std::make_pair(4, std::make_shared<CK3::Title>(titleCStream, 4));

	// Crosslink them.
	c.second->loadDJVassals(std::map{b1, b2, b3});

	try
	{
		auto test = c.second->getBuildingWeight(devWeightMapper);
		FAIL();
	}
	catch (const std::runtime_error& e)
	{
		ASSERT_STREQ("County c_county has no clay?", e.what());
	}

	std::stringstream landedStream;
	landedStream << "c_county = { }\n";
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedStream);
	auto lc = landedTitles.getFoundTitles().find("c_county")->second;
	c.second->loadClay(lc);

	try
	{
		auto test = c.second->getBuildingWeight(devWeightMapper);
		FAIL();
	}
	catch (const std::runtime_error& e)
	{
		ASSERT_STREQ("County c_county has no county in its clay?", e.what());
	}
}

TEST(CK3World_TitleTests, buildingWeighWithoutProvincefulBaronyClaysThrowsExceptions)
{
	std::stringstream devWeightMapperStream;
	devWeightMapperStream << "dev_from_building = 0.1\n";
	devWeightMapperStream << "dev_from_holding = 0.2\n";
	devWeightMapperStream << "dev_from_development = 0.5\n";
	auto devWeightMapper = mappers::DevWeightsMapper(devWeightMapperStream);

	std::stringstream titleB1Stream;
	titleB1Stream << "key=b_barony1\n";
	std::pair<long long, std::shared_ptr<CK3::Title>> b1 = std::make_pair(1, std::make_shared<CK3::Title>(titleB1Stream, 1));
	std::stringstream titleB2Stream;
	titleB2Stream << "key=b_barony2\n";
	std::pair<long long, std::shared_ptr<CK3::Title>> b2 = std::make_pair(2, std::make_shared<CK3::Title>(titleB2Stream, 2));
	std::stringstream titleB3Stream;
	titleB3Stream << "key=b_barony3\n";
	std::pair<long long, std::shared_ptr<CK3::Title>> b3 = std::make_pair(3, std::make_shared<CK3::Title>(titleB3Stream, 3));
	std::stringstream titleCStream;
	titleCStream << "key=c_county\nde_jure_vassals={1 2 3}\n";
	std::pair<long long, std::shared_ptr<CK3::Title>> c = std::make_pair(4, std::make_shared<CK3::Title>(titleCStream, 4));

	std::stringstream landedStream;
	landedStream << "c_county = { }\n";
	CK3::LandedTitles landedTitles;
	landedTitles.loadTitles(landedStream);
	auto lc = landedTitles.getFoundTitles().find("c_county")->second;

	std::stringstream countyDetailStream;
	countyDetailStream << "development = 7\n";
	auto countyDetail = std::pair("c_county", std::make_shared<CK3::CountyDetail>(countyDetailStream));

	// Crosslink them.
	c.second->loadDJVassals(std::map{b1, b2, b3});
	lc->loadCountyDetails(countyDetail);
	c.second->loadClay(lc);

	try
	{
		auto test = c.second->getBuildingWeight(devWeightMapper);
		FAIL();
	}
	catch (const std::runtime_error& e)
	{
		ASSERT_STREQ("Supposed barony b_barony1 of c_county has no clay?", e.what());
	}

	std::stringstream landedStream2;
	landedStream2 << "b_barony1 = { province = 12 } \n";
	landedStream2 << "b_barony2 = { province = 13 } \n";
	landedStream2 << "b_barony3 = { province = 14 } \n";
	landedTitles.loadTitles(landedStream2);
	auto lb1 = landedTitles.getFoundTitles().find("b_barony1")->second;
	auto lb2 = landedTitles.getFoundTitles().find("b_barony2")->second;
	auto lb3 = landedTitles.getFoundTitles().find("b_barony3")->second;
	b1.second->loadClay(lb1);
	b2.second->loadClay(lb2);
	b3.second->loadClay(lb3);

	try
	{
		auto test = c.second->getBuildingWeight(devWeightMapper);
		FAIL();
	}
	catch (const std::runtime_error& e)
	{
		ASSERT_STREQ("Barony b_barony1 of c_county has no clay province?", e.what());
	}
}

TEST(CK3World_TitleTests, dfVassalsCanBeRelinked)
{
	std::stringstream charInput;
	charInput << "11={\n";
	charInput << "\tfirst_name = \"bob spongepants\"\n";
	charInput << "\tlanded_data = {\n";
	charInput << "\t\tdomain = { 3 4 1 2 }\n"; // guy holds 4 titles. first is primary, then listed in order of importance.
	charInput << "\t\trealm_capital = 1\n";	 // this is his capital, a county (in game a barony but we're abstracting).
	charInput << "\t}\n";
	charInput << "}\n";
	CK3::Characters characters;
	characters.loadCharacters(charInput);

	std::stringstream titleInput;
	titleInput << "1 = { key = c_county1 holder = 11 de_facto_liege = 3 de_jure_liege = 3 }\n";	// this one is his capital.
	titleInput << "2 = { key = c_county2 holder = 11 de_facto_liege = 3 de_jure_liege = 4  }\n"; // this one links to his primary title. -> This is wrong.
	titleInput << "3 = { key = d_duchy1 holder = 11 de_jure_vassals = { 1 } }\n";						// this is primary title.
	titleInput << "4 = { key = d_duchy2 holder = 11 de_jure_vassals = { 2 } }\n";						// this is secondary duchy.
	CK3::Titles titles(titleInput);

	// Fist we put characters into titles.
	titles.linkCharacters(characters);
	// And titles into characters.
	characters.linkTitles(titles);
	// And titles into titles.
	titles.linkTitles();

	// then we tell the primary title to relink wrongly linked vassals.
	const auto& duchy1 = titles.getTitles().find("d_duchy1")->second;
	duchy1->relinkDeFactoVassals();

	// now let's grab the county2
	const auto& county2 = titles.getTitles().find("c_county2")->second;

	ASSERT_EQ(4, county2->getDFLiege()->first);											// it is properly linked to duchy2 now.
	ASSERT_EQ("d_duchy2", county2->getDFLiege()->second->getName());				// link works
	ASSERT_EQ(1, duchy1->getDFVassals().size());											// duchy1 has only 1 dfvassal
	ASSERT_EQ("c_county1", duchy1->getDFVassals().begin()->second->getName()); // and that's county1.

	// and finally
	const auto& duchy2 = titles.getTitles().find("d_duchy2")->second;
	ASSERT_EQ(1, duchy2->getDFVassals().size());											// duchy2 has the vassal registered
	ASSERT_EQ("c_county2", duchy2->getDFVassals().begin()->second->getName()); // and linked.
}

TEST(CK3World_TitleTests, DFLiegeAndDJLiegeSetCanBeChecked)
{
	std::stringstream titleInput;
	CK3::Title title(titleInput, 1);

	ASSERT_FALSE(title.areDFLiegeAndDJLiegeSet());

	title.loadDFLiege(std::pair(2, nullptr));
	ASSERT_FALSE(title.areDFLiegeAndDJLiegeSet());

	title.loadDJLiege(std::pair(3, nullptr));
	ASSERT_TRUE(title.areDFLiegeAndDJLiegeSet());
}

TEST(CK3World_TitleTests, DFLiegeAndDJLiegeSameCanBeChecked)
{
	std::stringstream titleInput;
	titleInput << "key = c_county1\n";
	titleInput << "holder = 11\n";
	titleInput << "de_facto_liege = 2\n";
	titleInput << "de_jure_liege = 3\n";
	CK3::Title title(titleInput, 1);

	ASSERT_TRUE(title.areDFLiegeAndDJLiegeSet()); // They are set.

	ASSERT_FALSE(title.areDFLiegeAndDJLiegeSame()); // Not the same.

	title.loadDFLiege(std::pair(3, nullptr)); // setting to same

	ASSERT_TRUE(title.areDFLiegeAndDJLiegeSame());
}

TEST(CK3World_TitleTests, isHolderSetCanBeChecked)
{
	std::stringstream titleInput;
	titleInput << "key = c_county1\n";
	CK3::Title title(titleInput, 1);

	ASSERT_FALSE(title.isHolderSet()); // No Holder

	title.loadHolder(std::pair(1, nullptr));

	ASSERT_TRUE(title.isHolderSet()); // Yes holder.
}

TEST(CK3World_TitleTests, isHolderLinkedCanBeChecked)
{
	std::stringstream titleInput;
	titleInput << "key = c_county1\n";
	titleInput << "holder = 12\n";
	CK3::Title title(titleInput, 1);

	ASSERT_TRUE(title.isHolderSet());	  // Yes Holder
	ASSERT_FALSE(title.isHolderLinked()); // Not linked

	auto holder = std::make_shared<CK3::Character>(titleInput, 12); // Using any stream just for init.
	title.loadHolder(std::pair(static_cast<long long>(12), holder));

	ASSERT_TRUE(title.isHolderLinked()); // Linked
}

TEST(CK3World_TitleTests, doesHolderHaveCharacterDomainCanBeChecked)
{
	std::stringstream titleInput;
	titleInput << "key = c_county1\n";
	titleInput << "holder = 12\n";
	CK3::Title title(titleInput, 1);
	auto dummyHolder = std::make_shared<CK3::Character>(titleInput, 12); // Using any stream just for init.
	title.loadHolder(std::pair(static_cast<long long>(12), dummyHolder));

	ASSERT_TRUE(title.isHolderSet());			  // Yes Holder
	ASSERT_TRUE(title.isHolderLinked());		  // Yes linked
	ASSERT_FALSE(title.doesHolderHaveCharacterDomain()); // No Domain

	// Make a character with a domain
	
	std::stringstream charInput;
	charInput << "first_name = \"bob spongepants\"\n";
	charInput << "landed_data = {\n";
	charInput << "\tdomain = { 3 4 1 2 }\n";
	charInput << "}\n";
	auto holder = std::make_shared<CK3::Character>(charInput, 12);

	title.loadHolder(std::pair(static_cast<long long>(12), holder));
	
	ASSERT_TRUE(title.doesHolderHaveCharacterDomain()); // Yes Domain
}

TEST(CK3World_TitleTests, titleNamesFromHoldersDomainCanBeGathered)
{
	std::stringstream charInput;
	charInput << "11={\n";
	charInput << "\tfirst_name = \"bob spongepants\"\n";
	charInput << "\tlanded_data = {\n";
	charInput << "\t\tdomain = { 3 4 1 2 }\n";
	charInput << "\t\trealm_capital = 1\n";
	charInput << "\t}\n";
	charInput << "}\n";
	CK3::Characters characters;
	characters.loadCharacters(charInput);
	
	std::stringstream titleInput;
	titleInput << "1 = { key = c_county1 holder = 11 de_facto_liege = 3 de_jure_liege = 3 }\n";
	titleInput << "2 = { key = c_county2 holder = 11 de_facto_liege = 3 de_jure_liege = 4  }\n";
	titleInput << "3 = { key = d_duchy1 holder = 11 de_jure_vassals = { 1 } }\n";
	titleInput << "4 = { key = d_duchy2 holder = 11 de_jure_vassals = { 2 } }\n";
	CK3::Titles titles(titleInput);

	titles.linkCharacters(characters);
	characters.linkTitles(titles);
	titles.linkTitles();

	// Now, grab any title.
	const auto& anyTitle = titles.getTitles().find("d_duchy2")->second;
	
	ASSERT_EQ(4, anyTitle->getTitleNamesFromHolderDomain().size());
	ASSERT_TRUE(anyTitle->getTitleNamesFromHolderDomain().contains("c_county1"));
	ASSERT_TRUE(anyTitle->getTitleNamesFromHolderDomain().contains("c_county2"));
	ASSERT_TRUE(anyTitle->getTitleNamesFromHolderDomain().contains("d_duchy1"));
	ASSERT_TRUE(anyTitle->getTitleNamesFromHolderDomain().contains("d_duchy2"));
}

TEST(CK3World_TitleTests, dynamicTitleRankCanBeAssigned)
{
	std::stringstream input;
	CK3::Title theTitle(input, 1);
	theTitle.setDynamicLevel(CK3::LEVEL::EMPIRE);

	ASSERT_EQ(CK3::LEVEL::EMPIRE, theTitle.getLevel());

	theTitle.setDynamicLevel(CK3::LEVEL::DUCHY);

	ASSERT_EQ(CK3::LEVEL::DUCHY, theTitle.getLevel());
}
