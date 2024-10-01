
using System.Collections.Generic;
using commonItems;

namespace CK3ToEU4.CK3.Religions;

class Religion
{
	public Religion()
	{
	}

	public Religion(BufferedReader reader, long theID)
	{
		ID = theID;
		
		var parser = new Parser();
		registerKeys(parser);
		parser.ParseStream(reader);
	}

	public long ID { get; private set; } = 0;
	public string getName() { return tag; }
	public string getFamily() { return family; }
	public IReadOnlyDictionary<long, Faith?> getFaiths() => faiths;

	public void loadFaiths(Dictionary<long, Faith?> theFaiths) { faiths = theFaiths; }

	
	private void registerKeys(Parser parser)
	{
		parser.RegisterKeyword("tag", reader => {
			tag = reader.GetString();
		});
		parser.RegisterKeyword("family", reader => {
			family = reader.GetString();
		});
		parser.RegisterKeyword("faiths", reader => {
			foreach (var faithId in reader.GetLongs())
			{
				faiths.Add(faithId, null);
			}
				
		});
		parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}


	
	private string tag;
	private string family;
	private Dictionary<long, Faith?> faiths;
};