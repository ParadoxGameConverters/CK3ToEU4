
namespace CK3
{
class Dynasties
{
  public Dynasties()
  {
  }

  public Dynasties(BufferedReader reader)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}

	public const auto& getDynasties() const { return dynasties; }
	public auto getHouses() { return std::move(houses); }

	public void linkCoats(const CoatsOfArms& coats)
	{
		auto counter = 0;
		const auto& coatData = coats.getCoats();
		for (const auto& dynasty: dynasties)
		{
			if (!dynasty.second->getCoA())
				continue;
			const auto& coatDataItr = coatData.find(dynasty.second->getCoA()->first);
			if (coatDataItr != coatData.end())
			{
				dynasty.second->loadCoat(*coatDataItr);
				++counter;
			}
			else
			{
				Log(LogLevel::Warning) << "Dynasty " + std::to_string(dynasty.first) + " has CoA " + std::to_string(dynasty.second->getCoA()->first) +
														" which has no definition!";
			}
		}
		Log(LogLevel::Info) << "<> " << counter << " dynasties updated.";
	}

	private void registerKeys()
	{
		registerKeyword("dynasty_house", reader => {
			houses = Houses(theStream);
		});
		registerKeyword("dynasties", reader => {
			dynasties = Dynasties(theStream).getDynasties();
		});
		registerRegex(R"(\d+)", [this](const string& gameID, std::istream& theStream) {
			const auto suspiciousItem = commonItems::stringOfItem(theStream).getString();
			if (suspiciousItem.find('{') != string::npos)
			{
				auto dynastyStream = stringstream(suspiciousItem);
				const auto newDynasty = std::make_shared<Dynasty>(dynastyStream, std::stoll(gameID));
				dynasties.insert(std::pair(newDynasty->getGameID(), newDynasty));
			}
		});
		registerRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}

	private Houses houses;
	private Dictionary<long, std::shared_ptr<Dynasty>> dynasties;
};
}