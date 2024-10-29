using System.Collections.Generic;
using commonItems;
using Open.Collections;

namespace CK3ToEU4.CK3.Flags;

class Flags
{
	public Flags()
	{
	}

	public Flags(BufferedReader reader)
	{
		var parser = new Parser();
		RegisterKeys(parser);
		parser.ParseStream(reader);

		if (itemType == "flag" && incomingFlag.Length > 0)
		{
			flags.Add(incomingFlag);
		}
	}

	public IReadOnlySet<string> getFlags() { return flags; }

	private void RegisterKeys(Parser parser)
	{
		parser.RegisterKeyword("list", reader => {
			foreach (var blob in new BlobList(reader).Blobs)
			{
				var blobReader = new BufferedReader(blob);
				var scraper = new Flags(blobReader);
				var foundFlags = scraper.getFlags();
				flags.AddRange(foundFlags);
			}
		});
		parser.RegisterKeyword("item", reader => {
			var scraper = new Flags(reader);
			var foundFlags = scraper.getFlags();
			flags.AddRange(foundFlags);
		});
		parser.RegisterKeyword("flag", reader => {
			incomingFlag = reader.GetString();
		});
		parser.RegisterKeyword("type", reader => {
			itemType = reader.GetString();
		});
		parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}
	
	private string itemType = string.Empty;
	private string incomingFlag = string.Empty;
	private readonly HashSet<string> flags = [];
}