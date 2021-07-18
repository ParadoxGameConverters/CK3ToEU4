#include "CoatOfArms.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::CoatOfArms::CoatOfArms(std::istream& theStream, long long theID): ID(theID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::CoatOfArms::registerKeys()
{
	registerKeyword("pattern", [this](const std::string& unused, std::istream& theStream) {
		pattern = commonItems::singleString(theStream).getString();
	});
	registerKeyword("color1", [this](const std::string& unused, std::istream& theStream) {
		try
		{
			color1 = laFabricaDeColor.getColor(theStream);
		}
		catch (std::exception& e)
		{
			Log(LogLevel::Warning) << e.what() << " - sidestepping with black.";
			color1 = commonItems::Color(std::array<int, 3>{0, 0, 0});
		}
	});
	registerKeyword("color2", [this](const std::string& unused, std::istream& theStream) {
		try
		{
			color2 = laFabricaDeColor.getColor(theStream);
		}
		catch (std::exception& e)
		{
			Log(LogLevel::Warning) << e.what() << " - sidestepping with black.";
			color2 = commonItems::Color(std::array<int, 3>{0, 0, 0});
		}
	});
	registerKeyword("color3", [this](const std::string& unused, std::istream& theStream) {
		try
		{
			color3 = laFabricaDeColor.getColor(theStream);
		}
		catch (std::exception& e)
		{
			Log(LogLevel::Warning) << e.what() << " - sidestepping with black.";
			color3 = commonItems::Color(std::array<int, 3>{0, 0, 0});
		}
	});
	registerKeyword("textured_emblem", [this](const std::string& unused, std::istream& theStream) {
		texturedEmblems.emplace_back(Emblem(theStream));
	});
	registerKeyword("colored_emblem", [this](const std::string& unused, std::istream& theStream) {
		coloredEmblems.emplace_back(Emblem(theStream));
	});
	registerKeyword("sub", [this](const std::string& unused, std::istream& theStream) {
		subs.emplace_back(std::make_shared<CoatOfArms>(theStream, 0));
	});
	registerKeyword("instance", [this](const std::string& unused, std::istream& theStream) {
		auto instance = EmblemInstance(theStream);
		if (instance.getOffset().empty())
			instance.defaultOffset();
		instances.emplace_back(instance);
	});
	registerKeyword("parent", [this](const std::string& unused, std::istream& theStream) {
		parent = std::make_pair(commonItems::singleString(theStream).getString(), nullptr);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
