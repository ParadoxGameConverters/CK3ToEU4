#ifndef CK3_RELIGION_H
#define CK3_RELIGION_H
#include "Parser.h"

namespace CK3
{
class Faith;
class Religion: commonItems::parser
{
  public:
	Religion() = default;
	Religion(std::istream& theStream, long long theID): ID(theID)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}

	[[nodiscard]] auto getID() const { return ID; }
	[[nodiscard]] const auto& getName() const { return tag; }
	[[nodiscard]] const auto& getFamily() const { return family; }
	[[nodiscard]] const auto& getFaiths() const { return faiths; }

	void loadFaiths(const std::map<long long, std::shared_ptr<Faith>>& theFaiths) { faiths = theFaiths; }

  private:
	void registerKeys()
	{
		registerKeyword("tag", [this](const std::string& unused, std::istream& theStream) {
			tag = commonItems::singleString(theStream).getString();
		});
		registerKeyword("family", [this](const std::string& unused, std::istream& theStream) {
			family = commonItems::singleString(theStream).getString();
		});
		registerKeyword("faiths", [this](const std::string& unused, std::istream& theStream) {
			for (auto faith: commonItems::llongList(theStream).getLlongs())
				faiths.insert(std::pair(faith, nullptr));
		});
		registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	}


	long long ID = 0;
	std::string tag;
	std::string family;
	std::map<long long, std::shared_ptr<Faith>> faiths;
};
} // namespace CK3

#endif // CK3_RELIGION_H
