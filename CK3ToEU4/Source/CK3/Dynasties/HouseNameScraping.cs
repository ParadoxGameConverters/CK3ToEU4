
namespace CK3
{
class HouseNameScraping
{
	public HouseNameScraping() {}
	public HouseNameScraping(BufferedReader reader)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}
	public const auto& getPrefix() const { return prefix; }
	public const auto& getName() const { return name; }

  private:
	void registerKeys()
	{
		registerKeyword("name", [this](BufferedReader reader) {
			name = reader.GetString();
		});
		registerKeyword("prefix", [this](BufferedReader reader) {
			prefix = reader.GetString();
		});
		registerRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}

	string name;
	string prefix;
};
}