
using CK3;

namespace CK3ToEU4.CK3.Religions;

class Religions
{
	public:
	explicit Religions(BufferedReader reader)
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
			Dictionary<long, std::shared_ptr<global::CK3.Faith>> replacementMap;

			for (const auto& faith: religionFaiths)
			{
				const auto& faithDataItr = faithData.find(faith.first);
				if (faithDataItr != faithData.end())
				{
					replacementMap.insert(*faithDataItr);
				}
				else
				{
					throw new Exception("Religion " + religion.second->getName() + " has faith " + std::to_string(faith.first) + " which has no definition!");
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
		registerRegex(R"(\d+)", [this](const string& faithID, std::istream& theStream) {
			auto newReligion = std::make_shared<Religion>(theStream, std::stoll(faithID));
			religions.insert(std::pair(newReligion->getID(), newReligion));
		});
		registerKeyword("religions", reader => {
			religions = Religions(theStream).getReligions();
		});
		registerKeyword("faiths", reader => {
			faiths = Faiths(theStream);
		});
		registerRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}

	Dictionary<long, std::shared_ptr<Religion>> religions;
	Faiths faiths;
};