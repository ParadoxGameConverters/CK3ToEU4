#include "Religions.h"
#include "Religion.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Religions::Religions(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Religions::registerKeys()
{
	registerRegex(R"(\d+)", [this](const std::string& faithID, std::istream& theStream) {
		auto newReligion = std::make_shared<Religion>(theStream, std::stoll(faithID));
		religions.insert(std::pair(newReligion->getID(), newReligion));
	});
	registerKeyword("religions", [this](const std::string& unused, std::istream& theStream) {
		religions = Religions(theStream).getReligions();
	});
	registerKeyword("faiths", [this](const std::string& unused, std::istream& theStream) {
		faiths = Faiths(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void CK3::Religions::linkFaiths(const Faiths& theFaiths)
{
	auto counter = 0;
	const auto& faithData = theFaiths.getFaiths();
	for (const auto& religion: religions)
	{
		const auto& religionFaiths = religion.second->getFaiths();
		std::map<long long, std::shared_ptr<Faith>> replacementMap;
		
		for (const auto& faith: religionFaiths)
		{
			const auto& faithDataItr = faithData.find(faith.first);
			if (faithDataItr != faithData.end())
			{
				replacementMap.insert(*faithDataItr);
			}
			else
			{
				throw std::runtime_error(
					 "Religion " + religion.second->getName() + " has faith " + std::to_string(faith.first) + " which has no definition!");
			}			
		}
		religion.second->loadFaiths(replacementMap);
		++counter;
	}
	Log(LogLevel::Info) << "<> " << counter << " religions updated.";
}
