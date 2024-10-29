using commonItems;

namespace CK3ToEU4.Mappers.TraitScraper;

class TraitScraping
{
	public TraitScraping() {}
	public TraitScraping(std::istream& theStream)
	{
		var parser = new Parser();
		registerKeys(parser);
		parser.ParseStream(reader);
	}

	public int getIndex() { return index; }

	private void registerKeys(Parser parser)
	{
		parser.RegisterKeyword("index", [this](const std::string& personalityName, std::istream& theStream) {
			index = commonItems::singleInt(theStream).getInt();
		});
		parser.RegisterRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	}

	private int index = 0;
};