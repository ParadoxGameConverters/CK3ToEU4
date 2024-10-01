
namespace CK3
{
class HouseNameScraper
{
  public:
	HouseNameScraper() = default;
	void loadHouseDetails(BufferedReader reader)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}
	void loadHouseDetails(const string& filename)
	{
		registerKeys();
		parseFile(filename);
		clearRegisteredKeywords();
	}

	[[nodiscard]] std::optional<string> getNameForKey(const string& houseKey)
	{
		if (houseNames.contains(houseKey))
			return houseNames.at(houseKey).getName();
		return std::nullopt;
	}
	[[nodiscard]] std::optional<string> getPrefixForKey(const string& houseKey)
	{
		if (houseNames.contains(houseKey))
			return houseNames.at(houseKey).getPrefix();
		return std::nullopt;
	}
	[[nodiscard]] const auto& getHouseNames() const { return houseNames; }

  private:
	void registerKeys()
	{
		registerRegex(commonItems::catchallRegex, [this](const string& houseKey, std::istream& theStream) {
			const auto houseDetails = HouseNameScraping(theStream);
			houseNames[houseKey] = houseDetails;
		});
	}

	Dictionary<string, HouseNameScraping> houseNames;
};
}