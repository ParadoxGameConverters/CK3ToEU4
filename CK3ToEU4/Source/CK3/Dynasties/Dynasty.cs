
namespace CK3
{
class Dynasty
{
	public Dynasty() = default;
	public Dynasty(std::istream& theStream, long theGameID)
	{
		gameID = theGameID;
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}

	public auto getGameID() const { return gameID; }
	public auto isAppropriateRealmName() const { return appropriateRealmName; }
	public const auto& getDynID() const { return dynID; }
	public const auto& getCoA() const { return coa; }

	public void loadCoat(const KeyValuePair<long, std::shared_ptr<CoatOfArms>>& coat) { coa = coat; }


	private void registerKeys()
	{
		registerKeyword("key", reader => {
			dynID = reader.GetString();
		});
		registerKeyword("good_for_realm_name", reader => {
			appropriateRealmName = reader.GetString() == "yes";
		});
		registerKeyword("coat_of_arms_id", reader => {
			coa = std::make_pair(reader.GetLong(), nullptr);
		});
		registerRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}

	private string dynID;	 // this is actual key, looks like an int but is actually string.
	private long gameID = 0; // this is savegame key
	private std::optional<KeyValuePair<long, std::shared_ptr<CoatOfArms>>> coa;
	private bool appropriateRealmName = false;
};
}