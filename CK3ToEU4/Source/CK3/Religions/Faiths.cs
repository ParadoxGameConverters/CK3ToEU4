#ifndef CK3_FAITHS_H
#define CK3_FAITHS_H
#include "../Titles/LandedTitles.cs"
#include "../Titles/Title.h"
#include "../Titles/Titles.cs"
#include "Parser.h"

namespace CK3
{
class Faith;
class Religions;
class Faiths: commonItems::parser
{
  public:
	Faiths() = default;
	public Faiths(BufferedReader reader)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}

	[[nodiscard]] const auto& getFaiths() const { return faiths; }

	void linkReligions(const Religions& religions, const Titles& titles)
	{
		auto counter = 0;
		const auto& religionData = religions.getReligions();
		Dictionary<string, string> religiousHeadList; // ID, Title
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
				throw new Exception(
					 "Faith " + faith.second->getName() + " has religion " + std::to_string(faith.second->getReligion().first) + " which has no definition!");
			}
		}
		Log(LogLevel::Info) << "<> " << counter << " faiths updated.";
	}

  private:
	void registerKeys()
	{
		registerRegex(R"(\d+)", [this](const string& faithID, std::istream& theStream) {
			auto newFaith = std::make_shared<Faith>(theStream, std::stoll(faithID));
			faiths.insert(std::pair(newFaith->getID(), newFaith));
		});
		registerRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}

	Dictionary<long, std::shared_ptr<Faith>> faiths;
};
} // namespace CK3

#endif // CK3_FAITHS_H
