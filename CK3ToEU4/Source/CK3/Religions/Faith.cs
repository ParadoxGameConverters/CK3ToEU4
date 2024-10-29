using System.Collections.Generic;
using commonItems;
using commonItems.Colors;

namespace CK3ToEU4.CK3.Religions;

class Faith
{
	public Faith()
	{
	}

	public Faith(BufferedReader reader, long theID, ColorFactory colorFactory)
	{
		ID = theID;
		
		var parser = new Parser();
		RegisterKeys(parser, colorFactory);
		parser.ParseStream(reader);
	}

	public long ID { get; private set; } = 0;
	public string getName() { return tag; }
	public Color? Color { return color; }
	public IReadOnlyList<string> getDoctrines() { return doctrines; }
	public KeyValuePair<long, Religion?> getReligion() { return religion; }
	public string getReligiousHead() { return religiousHead; }
	public string getCustomName() { return customName; }
	public string getCustomAdj() { return customAdjective; }
	public string getDescription() { return description; }
	public string getTemplate() { return religionTemplate; }
	public string getIconPath() { return iconPath; }
	public bool getReformedFlag() { return reformedFlag; }

	public void setReligiousHead(string newHead) { religiousHead = newHead; }
	public void loadReligion(KeyValuePair<long, Religion?> theReligion) { religion = theReligion; }

	
	private void RegisterKeys(Parser parser, ColorFactory colorFactory)
	{
		parser.RegisterKeyword("tag", reader => {
			tag = reader.GetString();
		});
		parser.RegisterKeyword("doctrine", reader => {
			doctrines.Add(reader.GetString());
		});
		parser.RegisterKeyword("religion", reader => {
			religion = new(reader.GetLong(), null);
		});
		parser.RegisterKeyword("color", reader => {
			color = colorFactory.GetColor(reader);
		});
		parser.RegisterKeyword("template", reader => {
			religionTemplate = reader.GetString();
		});
		parser.RegisterKeyword("name", reader => {
			customName = reader.GetString();
		});
		parser.RegisterKeyword("adjective", reader => {
			customAdjective = reader.GetString();
		});
		parser.RegisterKeyword("religious_head", reader => {
			religiousHead = reader.GetString();
		});
		parser.RegisterKeyword("desc", reader => {
			description = reader.GetString();
		});
		parser.RegisterKeyword("icon", reader => {
			iconPath = reader.GetString();
		});
		parser.RegisterKeyword("variables", reader => {
			if (reader.GetStringOfItem().ToString().Contains("has_been_reformed"))
			{
				reformedFlag = true;
			}
		});
		parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}


	private bool reformedFlag = false;
	private string tag;
	private string religionTemplate;
	private string iconPath;
	private string customName;
	private string customAdjective;
	private string description;
	private string religiousHead;
	private Color? color;
	private readonly List<string> doctrines = []; // This is a vector in order to keep order consistent. We want the first things read (tenets) to be the first things output, ALWAYS
	private KeyValuePair<long, Religion?> religion;
}