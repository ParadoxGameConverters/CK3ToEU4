
using System.Collections.Generic;
using CK3ToEU4.CK3.Cultures;
using commonItems;

namespace CK3ToEU4.CK3.Geography;

public class CountyDetail
{
	public CountyDetail() {}
	public CountyDetail(BufferedReader reader)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}

	public auto getDevelopment() const { return development; }
	public const auto& getCulture() const { return culture; }
	public const auto& getFaith() const { return faith; }
	public const auto& isDeJureHRE() const { return deJureHRE; }

	public void loadCulture(const KeyValuePair<long, std::shared_ptr<Culture>>& theCulture) { culture = theCulture; }
	public void loadFaith(const KeyValuePair<long, std::shared_ptr<Faith>>& theFaith) { faith = theFaith; }
	public void setDeJureHRE() { deJureHRE = true; }

	private void registerKeys(Parser parser)
	{
		parser.RegisterKeyword("development", reader => {
			development = reader.GetInt();
		});
		parser.RegisterKeyword("culture", reader => {
			culture = new(reader.GetLong(), null);
			if (culture.first == 4294967295)
				culture.first = 0;
		});
		parser.RegisterKeyword("faith", reader => {
			faith = std::make_pair(reader.GetLong(), nullptr);
			if (faith.first == 4294967295)
				faith.first = 0;
		});
		parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}


	private int development = 0;
	private KeyValuePair<long, Culture?> culture;
	private KeyValuePair<long, Faith?> faith;
	private bool deJureHRE = false;
}