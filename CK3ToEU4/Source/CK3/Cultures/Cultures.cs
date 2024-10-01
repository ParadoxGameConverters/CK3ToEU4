using System.Collections.Generic;
using commonItems;

namespace CK3ToEU4.CK3.Cultures;

class CultureCollection
{
	public CultureCollection() {}
	public CultureCollection(BufferedReader reader)
	{
		var parser = new Parser();
		registerKeys(parser);
		parser.ParseStream(reader);
	}

	public IReadOnlyDictionary<long, Culture> Cultures => cultures;

	public void concoctCultures(LocalizationMapper localizationMapper, CultureMapper cultureMapper)
	{
		foreach (var  culture in cultures.Values)
		{
			culture.concoctCultureName(localizationMapper, cultureMapper, cultureNamingCounter);
		}
	}
	
	private void registerKeys(Parser parser)
	{
		parser.RegisterRegex(CommonRegexes.Integer, (reader, cultureId) => {
			var newCulture = new Culture(reader, long.Parse(cultureId));
			cultures.Add(newCulture.getID(), newCulture);
		});
		parser.RegisterKeyword("cultures", reader => {
			var scraper = new CultureCollection(reader);
			cultures = new(scraper.cultures);
		});
		parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}

	private Dictionary<long, Culture> cultures;
	private readonly Dictionary<string, int> cultureNamingCounter = [];
};