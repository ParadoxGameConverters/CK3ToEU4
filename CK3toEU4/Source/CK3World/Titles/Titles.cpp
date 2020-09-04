#include "Titles.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "Title.h"

CK3::Titles::Titles(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Titles::registerKeys()
{
	registerKeyword("landed_titles", [this](const std::string& unused, std::istream& theStream) {
		// A bit of recursion is good for the soul.
		const auto& tempTitles = Titles(theStream);
		titles = tempTitles.getTitles();
	});
	registerRegex(R"(\d+)", [this](const std::string& ID, std::istream& theStream) {
		// Incoming titles may not be actual titles but half-deleted junk.
		const auto& titleBlob = commonItems::singleItem(ID, theStream);
		if (titleBlob.find('{') != std::string::npos)
		{
			try
			{
				std::stringstream tempStream(titleBlob);
				auto newTitle = std::make_shared<Title>(tempStream, std::stoi(ID));
				if (!newTitle->getName().empty())
					titles.insert(std::pair(newTitle->getName(), newTitle));
			}
			catch (std::exception& e)
			{
				throw std::runtime_error("Title ID: " + ID + " is not a number? " + e.what());
			}
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
