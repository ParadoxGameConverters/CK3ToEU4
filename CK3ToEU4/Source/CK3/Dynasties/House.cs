
using System.Collections.Generic;
using CK3;
using commonItems;

namespace CK3ToEU4.CK3.Dynasties;

class House
{

	public House()
	{
	}

	public House(BufferedReader reader, long houseID)
	{
		ID = houseID;
		
		var parser = new Parser();
		registerKeys(parser);
		parser.ParseStream(reader);
	}
	public string getKey() { return key; }
	public string getName() { return name; }
	public string getLocalizedName() { return localizedName; }
	public string getPrefix() { return prefix; }
	public KeyValuePair<long, Dynasty?> getDynasty() { return dynasty; }
	public long ID { get; private set; } = 0;
	public KeyValuePair<long, Character?>? getHouseHead() { return houseHead; }

	public void loadDynasty(KeyValuePair<long, Dynasty?> theDynasty) { dynasty = theDynasty; }
	public void loadHouseHead(KeyValuePair<long, Character?> theHead) { houseHead = theHead; }

	public void resetHouseHead()
	{
		houseHead = null;
	}

	public void setName(string theName) { name = theName; }
	public void setPrefix(string thePrefix) { prefix = thePrefix; }

  
	private void registerKeys(Parser parser)
	{
		parser.RegisterKeyword("key", reader => {
			key = reader.GetString();
		});
		parser.RegisterKeyword("name", reader => {
			name = reader.GetString();
		});
		parser.RegisterKeyword("localized_name", reader => {
			localizedName = reader.GetString();
		});
		parser.RegisterKeyword("prefix", reader => {
			prefix = reader.GetString();
		});
		parser.RegisterKeyword("dynasty", reader => {
			dynasty = new(reader.GetLong(), null);
		});
		parser.RegisterKeyword("head_of_house", reader => {
			houseHead = new(reader.GetLong(), null);
		});
		parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}
	
	private string key;
	private string name;
	private string localizedName;
	private KeyValuePair<long, Dynasty?> dynasty;
	private string prefix;
	private KeyValuePair<long, Character?>? houseHead; // houses can have missing heads or dead people...
};