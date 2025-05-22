#include "CK3ReligionScraper.h"
#include "CK3ReligionScraping.h"
#include "CommonRegexes.h"
#include "Log.h"
#include <ranges>

void mappers::CK3ReligionScraper::scrapeCK3Religions(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Scraping CK3 religion hierarchy.";
	registerKeys();
	ParseGameFolder("common/religion/religions", modFS, {".txt"}, false);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << religionFaiths.size() << " religions.";
}

void mappers::CK3ReligionScraper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& religionName, std::istream& theStream) {
		religionFaiths.emplace(religionName, CK3ReligionScraping(theStream).getFaithNames());
	});
}

std::set<std::string> mappers::CK3ReligionScraper::getSimilarFaiths(const std::string& faith) const
{
	for (const auto& faiths: religionFaiths | std::views::values)
		if (faiths.contains(faith))
			return faiths;
	return {};
}
