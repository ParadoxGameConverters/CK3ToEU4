#include "Faiths.h"
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
		auto newFaith = std::make_shared<Faith>(theStream, std::stoi(faithID));
		faiths.insert(std::pair(newFaith->getID(), newFaith));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void CK3::Faiths::linkReligions(const Religions& religions)
{
	auto counter = 0;
	const auto& religionData = religions.getReligions();
	for (const auto& faith: faiths)
	{
		const auto& religionDataItr = religionData.find(faith.second->getReligion().first);
		if (religionDataItr != religionData.end())
		{
			faith.second->loadReligion(*religionDataItr);
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