#ifndef CK3_RELIGIONS_H
#define CK3_RELIGIONS_H
#include "Faiths.cs"
#include "Parser.h"

namespace CK3
{
class Religion;
class Religions: commonItems::parser
{
  public:
	explicit Religions(std::istream& theStream)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}

	[[nodiscard]] const auto& getReligions() const { return religions; }
	[[nodiscard]] auto getFaiths() { return std::move(faiths); } // Use this only once in World.cpp

	void linkFaiths(const Faiths& theFaiths)
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
					throw std::runtime_error("Religion " + religion.second->getName() + " has faith " + std::to_string(faith.first) + " which has no definition!");
				}
			}
			religion.second->loadFaiths(replacementMap);
			++counter;
		}
		Log(LogLevel::Info) << "<> " << counter << " religions updated.";
	}

  private:
	void registerKeys()
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

	std::map<long long, std::shared_ptr<Religion>> religions;
	Faiths faiths;
};
} // namespace CK3

#endif // CK3_RELIGIONS_H
