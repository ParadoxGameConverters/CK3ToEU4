#include "../CK3ToEU4/Source/Configuration/Configuration.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3ToEU4_ConfigurationTests, EU4PathDefaultsBlank)
{
	std::stringstream input("");
	const Configuration testConfiguration(input);

	ASSERT_EQ(testConfiguration.getEU4Path(), "");
}

TEST(CK3ToEU4_ConfigurationTests, EU4PathCanBeSet)
{
	std::stringstream input;
	input << "EU4directory = \"C:\\EU4Path\"";
	const Configuration testConfiguration(input);

	ASSERT_EQ(testConfiguration.getEU4Path(), "C:\\EU4Path");
}

TEST(CK3ToEU4_ConfigurationTests, CK3PathDefaultsBlank)
{
	std::stringstream input("");
	const Configuration testConfiguration(input);

	ASSERT_EQ(testConfiguration.getCK3Path(), "");
}

TEST(CK3ToEU4_ConfigurationTests, CK3PathCanBeSet)
{
	std::stringstream input;
	input << "CK3directory = \"C:\\CK3Path\"";
	const Configuration testConfiguration(input);

	ASSERT_EQ(testConfiguration.getCK3Path(), "C:\\CK3Path");
}

TEST(CK3ToEU4_ConfigurationTests, CK3DocPathDefaultsBlank)
{
	std::stringstream input("");
	const Configuration testConfiguration(input);

	ASSERT_EQ(testConfiguration.getCK3DocPath(), "");
}

TEST(CK3ToEU4_ConfigurationTests, CK3DocPathCanBeSet)
{
	std::stringstream input;
	input << "CK3DocDirectory = \"C:\\CK3Path\\Documents\"";
	const Configuration testConfiguration(input);

	ASSERT_EQ(testConfiguration.getCK3DocPath(), "C:\\CK3Path\\Documents");
}

TEST(CK3ToEU4_ConfigurationTests, SaveGamePathDefaultsBlank)
{
	std::stringstream input("");
	const Configuration testConfiguration(input);

	ASSERT_EQ(testConfiguration.getSaveGamePath(), "");
}

TEST(CK3ToEU4_ConfigurationTests, SaveGamePathCanBeSet)
{
	std::stringstream input;
	input << "SaveGame = \"C:\\CK3Path\\save games\\autosave.ck3\"";
	const Configuration testConfiguration(input);

	ASSERT_EQ(testConfiguration.getSaveGamePath(), "C:\\CK3Path\\save games\\autosave.ck3");
}

TEST(CK3ToEU4_ConfigurationTests, OutputNameDefaultsToSaveGameWithNoOutputName)
{
	std::stringstream input;
	input << "SaveGame = \"C:\\CK3Path\\save games\\autosave.ck3\"";

	const Configuration testConfiguration(input);

	ASSERT_EQ(testConfiguration.getOutputName(), "autosave");
}

TEST(CK3ToEU4_ConfigurationTests, OutputNameResistantToMixedSlashes)
{
	std::stringstream input;
	input << "SaveGame = \"C:\\CK3Path/save games/autosave.ck3\"";
	const Configuration testConfiguration(input);

	ASSERT_EQ(testConfiguration.getOutputName(), "autosave");
}

TEST(CK3ToEU4_ConfigurationTests, OutputNameReplacesSpacesAndMinuses)
{
	std::stringstream input;
	input << "SaveGame = \"C:\\CK3Path\\save games\\autosav - - . second e.ck3\"";

	const Configuration testConfiguration(input);

	ASSERT_EQ(testConfiguration.getOutputName(), "autosav_____._second_e");
}

TEST(CK3ToEU4_ConfigurationTests, OutputNameDefaultsToSaveGame)
{
	std::stringstream input;
	input << "output_name = \"\"\n";
	input << "SaveGame = \"C:\\CK3Path\\save games\\autosave.ck3\"";

	const Configuration testConfiguration(input);

	ASSERT_EQ(testConfiguration.getOutputName(), "autosave");
}

TEST(CK3ToEU4_ConfigurationTests, OutputNameCanBeSet)
{
	std::stringstream input;
	input << "output_name = \"override\"\n";
	input << "SaveGame = \"C:\\CK3Path\\save games\\autosave.ck3\"";

	const Configuration testConfiguration(input);

	ASSERT_EQ(testConfiguration.getOutputName(), "override");
}
