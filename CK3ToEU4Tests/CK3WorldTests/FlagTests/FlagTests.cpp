#include "../../CK3toEU4/Source/CK3World/Flags/Flags.h"
#include "gtest/gtest.h"
#include <sstream>

TEST(CK3World_FlagsTests, FlagsDefaultToBlank)
{
	std::stringstream input;
	const CK3::Flags flags(input);

	ASSERT_TRUE(flags.getFlags().empty());
}

TEST(CK3World_FlagsTests, UnbundledFlagsCanBeLoaded)
{
	std::stringstream input;
	input << "item = {\n";
	input << "\ttype = flag\n";
	input << "\tflag = \"a_flag\"\n";
	input << "}\n";
	input << "item = {\n";
	input << "\ttype = flag\n";
	input << "\tflag = \"b_flag\"\n";
	input << "}\n";

	const CK3::Flags flags(input);

	ASSERT_EQ(2, flags.getFlags().size());
	ASSERT_EQ(1, flags.getFlags().count("a_flag"));
	ASSERT_EQ(1, flags.getFlags().count("b_flag"));
}

TEST(CK3World_FlagsTests, MalformedFlagsAreIgnored)
{
	std::stringstream input;
	input << "item = {\n";
	input << "\ttype = flag\n"; // missing flag name
	input << "}\n";
	input << "item = {\n";
	input << "\tflag = \"b_flag\"\n"; // missing flag type
	input << "}\n";
	input << "item = {\n";
	input << "\ttype = flag\n";
	input << "\tflag = \"correct_flag\"\n";
	input << "}\n";

	const CK3::Flags flags(input);

	ASSERT_EQ(1, flags.getFlags().size());
	ASSERT_EQ(1, flags.getFlags().count("correct_flag"));
}

TEST(CK3World_FlagsTests, BundledFlagsCanBeLoaded)
{
	std::stringstream input;
	input << "list = {\n";
	input << "\titem = {\n";
	input << "\t\ttype = flag\n";
	input << "\t\tflag = \"a_flag\"\n";
	input << "\t}\n";
	input << "\titem = {\n";
	input << "\t\ttype = flag\n";
	input << "\t\tflag = \"b_flag\"\n";
	input << "\t}\n";
	input << "}\n";

	const CK3::Flags flags(input);

	ASSERT_EQ(2, flags.getFlags().size());
	ASSERT_EQ(1, flags.getFlags().count("a_flag"));
	ASSERT_EQ(1, flags.getFlags().count("b_flag"));
}
