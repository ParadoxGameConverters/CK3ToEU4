#include "ReligionGroupScraper.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "ReligionGroupScraping.h"

mappers::ReligionGroupScraper::ReligionGroupScraper()
{
	LOG(LogLevel::Info) << "-> Scraping religion hierarchy.";
	registerKeys();
	for (const auto& fileName: commonItems::GetAllFilesInFolder("blankmod/output/common/religions/"))
		parseFile("blankmod/output/common/religions/" + fileName);
	clearRegisteredKeywords();
	LOG(LogLevel::Info) << "<> Loaded " << religionGroups.size() << " religion groups.";
}

mappers::ReligionGroupScraper::ReligionGroupScraper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionGroupScraper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& religionGroupName, std::istream& theStream) {
		const auto religions = ReligionGroupScraping(theStream);
		const auto& foundReligions = religions.getReligionNames();
		religionGroups[religionGroupName].insert(foundReligions.begin(), foundReligions.end());
	});
}

std::optional<std::string> mappers::ReligionGroupScraper::getReligionGroupForReligion(const std::string& religion) const
{
	for (const auto& religionGroupItr: religionGroups)
		if (religionGroupItr.second.count(religion))
			return religionGroupItr.first;
	return std::nullopt;
}
