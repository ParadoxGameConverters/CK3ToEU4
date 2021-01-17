#include "Emblem.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Emblem::Emblem(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Emblem::registerKeys()
{
	registerKeyword("texture", [this](const std::string& unused, std::istream& theStream) {
		texture = commonItems::singleString(theStream).getString();
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
	registerKeyword("mask", [this](const std::string& unused, std::istream& theStream) {
		mask = commonItems::intList(theStream).getInts();
	});
	registerKeyword("instance", [this](const std::string& unused, std::istream& theStream) {
		auto instance = EmblemInstance(theStream);
		if (instance.getPosition().empty())
			instance.defaultPosition();
		instances.emplace_back(instance);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
