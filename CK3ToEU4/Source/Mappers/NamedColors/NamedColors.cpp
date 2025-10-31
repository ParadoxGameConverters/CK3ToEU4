#include "NamedColors.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

void mappers::NamedColors::loadColors(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::NamedColors::loadColors(const std::filesystem::path& filepath)
{
	registerKeys();
	parseFile(filepath);
	clearRegisteredKeywords();
}

void mappers::NamedColors::registerKeys()
{
	// If we get a color named "colors", we're f--d.
	registerKeyword("colors", [](const std::string& unused, std::istream& theStream) {
		auto loadedColors = NamedColors();
		loadedColors.loadColors(theStream);
	});
	registerRegex(R"(\s*\w*\ \{[ 0.]*\})", [](const std::string& junk, std::istream& theStream) {
		Log(LogLevel::Debug) << "Ignoring junk named colors: " << junk; // Fix for PDX incompetence.
	});
	registerRegex(commonItems::catchallRegex, [](const std::string& colorName, std::istream& theStream) {
		try
		{
			laFabricaDeColor.addNamedColor(colorName, theStream);
		}
		catch (std::exception& e)
		{
			Log(LogLevel::Warning) << "Issue parsing named colors: " << colorName << " : " << e.what();
		}
	});
}