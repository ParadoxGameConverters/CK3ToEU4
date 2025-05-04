#include "Confederations.h"
#include "CommonRegexes.h"
#include "Confederation.h"
#include "ParserHelpers.h"
#include <ranges>

CK3::Confederations::Confederations(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Confederations::registerKeys()
{
	databaseParser.registerRegex(R"(\d+)", [this](const std::string& confederationID, std::istream& theStream) {
		const auto questionableItem = commonItems::stringOfItem(theStream).getString();
		auto tempStream = std::stringstream(questionableItem);
		if (questionableItem.find('{') != std::string::npos)
		{
			try
			{
				auto newConfederation = std::make_shared<Confederation>(tempStream, std::stoll(confederationID));
				confederations.insert(std::pair(newConfederation->getID(), newConfederation));
			}
			catch (std::exception&)
			{
				Log(LogLevel::Warning) << "Invalid confederation blob: " << questionableItem << " at ID: " << confederationID;
			}
		}
	});
	databaseParser.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	registerKeyword("database", [this](std::istream& theStream) {
		databaseParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
