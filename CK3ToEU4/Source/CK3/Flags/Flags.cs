#ifndef CK3_FLAGS_H
#define CK3_FLAGS_H
#include <set>

#include "Parser.h"

namespace CK3
{
class Flags: commonItems::parser
{
  public:
	Flags() = default;
	explicit Flags(std::istream& theStream)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();

		if (itemType == "flag" && !incomingFlag.empty())
			flags.insert(incomingFlag);
	}

	[[nodiscard]] const auto& getFlags() const { return flags; }

  private:
	void registerKeys()
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


	std::string itemType;
	std::string incomingFlag;
	std::set<std::string> flags;
};
} // namespace CK3

#endif // CK3_FLAGS_H
