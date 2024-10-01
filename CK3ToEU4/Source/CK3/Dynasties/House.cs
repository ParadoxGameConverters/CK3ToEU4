
namespace CK3ToEU4.CK3.Dynasties;

class House
{

	public House()
	{
	}

	public House(std::istream& theStream, long housID)
	{
		houseID = housID;
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}
	public const auto& getKey() const { return key; }
	public const auto& getName() const { return name; }
	public const auto& getLocalizedName() const { return localizedName; }
	public const auto& getPrefix() const { return prefix; }
	public const auto& getDynasty() const { return dynasty; }
	public const auto& getID() const { return houseID; }
	public const auto& getHouseHead() const { return houseHead; }

	public void loadDynasty(const KeyValuePair<long, std::shared_ptr<Dynasty>>& theDynasty) { dynasty = theDynasty; }
	public void loadHouseHead(const KeyValuePair<long, std::shared_ptr<Character>>& theHead) { houseHead = theHead; }
	public void resetHouseHead() { houseHead.reset(); }

	public void setName(const string& theName) { name = theName; }
	public void setPrefix(const string& thePrefix) { prefix = thePrefix; }

  
	private void registerKeys()
	{
		registerKeyword("key", [this](BufferedReader reader) {
			key = reader.GetString();
		});
		registerKeyword("name", reader => {
			name = reader.GetString();
		});
		registerKeyword("localized_name", reader => {
			localizedName = reader.GetString();
		});
		registerKeyword("prefix", reader => {
			prefix = reader.GetString();
		});
		registerKeyword("dynasty", reader => {
			dynasty = std::make_pair(reader.GetLong(), nullptr);
		});
		registerKeyword("head_of_house", reader => {
			houseHead = std::make_pair(reader.GetLong(), nullptr);
		});
		registerRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}


	private long houseID = 0;
	private string key;
	private string name;
	private string localizedName;
	private KeyValuePair<long, std::shared_ptr<Dynasty>> dynasty;
	private string prefix;
	private std::optional<KeyValuePair<long, std::shared_ptr<Character>>> houseHead; // houses can have missing heads or dead people...
};