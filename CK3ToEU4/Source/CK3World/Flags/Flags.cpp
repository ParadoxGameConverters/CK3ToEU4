#include "Flags.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

// This file loads data flags ("flag_re_restored_antioch"), not graphical flags.
// Since game uses this type of syntax:
//		type=flag
//		flag="flag_formed_kingdom_of_aragon"
// we'll be loading only those flags that have type = flag set. Unsure if any other flag type exists.
// Flags in "data" section of save are ignored - those appear to be gameplay-session related and not really global.
//
// Flags are further complicated by being bundled in data groups. As of yet we do not know if
// name="unavailable_unique_decisions" { flag="flag_formed_kingdom_of_aragon" }
// means aragon is already formed, thus decision is unavailable (giving proper semantics to the flag we scrape) or
// aragon forming decision is simply unavailable, in which case the flag means the opposite of what we assume it to be.

CK3::Flags::Flags(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	if (itemType == "flag" && !incomingFlag.empty())
		flags.insert(incomingFlag);
}

void CK3::Flags::registerKeys()
{
	registerKeyword("list", [this](const std::string& unused, std::istream& theStream) {
		for (const auto& blob: commonItems::blobList(theStream).getBlobs())
		{
			auto blobStream = std::stringstream(blob);
			const auto scraper = Flags(blobStream);
			const auto& foundFlags = scraper.getFlags();
			flags.insert(foundFlags.begin(), foundFlags.end());
		}
	});
	registerKeyword("item", [this](const std::string& unused, std::istream& theStream) {
		const auto scraper = Flags(theStream);
		const auto& foundFlags = scraper.getFlags();
		flags.insert(foundFlags.begin(), foundFlags.end());
	});
	registerKeyword("flag", [this](const std::string& unused, std::istream& theStream) {
		incomingFlag = commonItems::singleString(theStream).getString();
	});
	registerKeyword("type", [this](const std::string& unused, std::istream& theStream) {
		itemType = commonItems::singleString(theStream).getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
