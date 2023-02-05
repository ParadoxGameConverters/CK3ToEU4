#include "Faiths.h"
#include "CommonRegexes.h"
#include "Faith.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "Religions.h"

CK3::Faiths::Faiths(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Faiths::registerKeys()
{
	registerRegex(R"(\d+)", [this](const std::string& faithID, std::istream& theStream) {
		auto newFaith = std::make_shared<Faith>(theStream, std::stoll(faithID));
		faiths.insert(std::pair(newFaith->getID(), newFaith));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void CK3::Faiths::linkReligions(const Religions& religions, const Titles& titles)
{
	auto counter = 0;
	const auto& religionData = religions.getReligions();
	std::map<std::string, std::string> religiousHeadList; // ID, Title
	for (const auto& title: titles.getTitles())
		if (title.second)
			religiousHeadList.emplace(std::to_string(title.second->getID()), title.first);
	for (const auto& faith: faiths)
	{
		const auto& religionDataItr = religionData.find(faith.second->getReligion().first);
		if (religionDataItr != religionData.end())
		{
			faith.second->loadReligion(*religionDataItr);
			if (religiousHeadList.contains(faith.second->getReligiousHead()))
				faith.second->setReligiousHead(religiousHeadList.at(faith.second->getReligiousHead()));
			++counter;
		}
		else
		{
			throw std::runtime_error(
				 "Faith " + faith.second->getName() + " has religion " + std::to_string(faith.second->getReligion().first) + " which has no definition!");
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " faiths updated.";
}