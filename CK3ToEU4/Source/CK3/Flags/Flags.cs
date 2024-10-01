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
	explicit Flags(BufferedReader reader)
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
		registerKeyword("list", reader => {
			for (const auto& blob: commonItems::blobList(theStream).getBlobs())
			{
				auto blobStream = stringstream(blob);
				const auto scraper = Flags(blobStream);
				const auto& foundFlags = scraper.getFlags();
				flags.insert(foundFlags.begin(), foundFlags.end());
			}
		});
		registerKeyword("item", reader => {
			const auto scraper = Flags(theStream);
			const auto& foundFlags = scraper.getFlags();
			flags.insert(foundFlags.begin(), foundFlags.end());
		});
		registerKeyword("flag", reader => {
			incomingFlag = reader.GetString();
		});
		registerKeyword("type", reader => {
			itemType = reader.GetString();
		});
		registerRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}


	string itemType;
	string incomingFlag;
	HashSet<string> flags;
};
} // namespace CK3

#endif // CK3_FLAGS_H
