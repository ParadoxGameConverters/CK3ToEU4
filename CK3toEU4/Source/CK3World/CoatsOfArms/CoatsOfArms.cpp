#include "CoatsOfArms.h"
#include "../Titles/Title.h"
#include "../Titles/Titles.h"
#include "CoatOfArms.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::CoatsOfArms::CoatsOfArms(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::CoatsOfArms::registerKeys()
{
	registerRegex(R"(\d+)", [this](const std::string& coaID, std::istream& theStream) {
		auto newCoA = std::make_shared<CoatOfArms>(theStream, std::stoll(coaID));
		coats.insert(std::pair(newCoA->getID(), newCoA));
	});
	registerKeyword("coat_of_arms_manager_database", [this](const std::string& unused, std::istream& theStream) {
		coats = CoatsOfArms(theStream).getCoats();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void CK3::CoatsOfArms::linkParents(const Titles& titles)
{
	auto counter = 0;
	const auto& titleData = titles.getTitles();
	for (const auto& coat: coats)
	{
		if (coat.second->getParent().first.empty())
			continue;
		const auto& titleDataItr = titleData.find(coat.second->getParent().first);
		if (titleDataItr != titleData.end())
		{
			if (!titleDataItr->second->getCoA())
				throw std::runtime_error("CoA " + std::to_string(coat.first) + " has parent " + coat.second->getParent().first + " which has no coat defined!");
			if (!coats.count(titleDataItr->second->getCoA()->first))
				throw std::runtime_error("CoA " + std::to_string(coat.first) + " has parent " + coat.second->getParent().first + " which has invalid coat defined!");
			coat.second->loadParent(std::make_pair(coat.second->getParent().first, coats[titleDataItr->second->getCoA()->first]));
			++counter;
		}
		else
		{
			throw std::runtime_error("CoA " + std::to_string(coat.first) + " has parent " + coat.second->getParent().first + " which is undefined!");
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " coats updated.";
}