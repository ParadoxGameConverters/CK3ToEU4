using commonItems;

namespace CK3ToEU4.CK3.Titles;

class DynamicTemplate
{
	public DynamicTemplate(BufferedReader reader)
	{
		var parser = new Parser();
		RegisterKeys(parser);
		parser.ParseStream(reader);
	}

	public string DynamicTitleKey { get; private set; } = string.Empty;
	public string DynamicTitleRank { get; private set; } = string.Empty;
		
	private void RegisterKeys(Parser parser)
	{
		parser.RegisterKeyword("key", reader => {
			DynamicTitleKey = reader.GetString();
		});
		parser.RegisterKeyword("tier", reader => {
			DynamicTitleRank = reader.GetString();
		});
		parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}
}

