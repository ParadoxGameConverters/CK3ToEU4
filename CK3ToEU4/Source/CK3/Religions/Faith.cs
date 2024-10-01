

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
		registerKeys(parser, colorFactory);
		parser.ParseStream(reader);
	}

	public const auto& getName() { return tag; }
	public const auto& getColor() { return color; }
	public const auto& getDoctrines() { return doctrines; }
	public const auto& getReligion() { return religion; }
	public const auto& getReligiousHead() { return religiousHead; }
	public auto getID() { return ID; }
	public const auto& getCustomName() { return customName; }
	public const auto& getCustomAdj() { return customAdjective; }
	public const auto& getDescription() { return description; }
	public const auto& getTemplate() { return religionTemplate; }
	public const auto& getIconPath() { return iconPath; }
	public const auto& getReformedFlag() { return reformedFlag; }

	public void setReligiousHead(const auto& newHead) { religiousHead = newHead; }
	public void loadReligion(const KeyValuePair<long, std::shared_ptr<Religion>>& theReligion) { religion = theReligion; }

	
	private void registerKeys(Parser parser, ColorFactory colorFactory)
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
			if (commonItems::stringOfItem(theStream).getString().find("has_been_reformed") != string::npos)
				reformedFlag = true;
		});
		parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}


	private bool reformedFlag = false;
	private long ID = 0;
	private string tag;
	private string religionTemplate;
	private string iconPath;
	private string customName;
	private string customAdjective;
	private string description;
	private string religiousHead;
	private Color? color;
	private List<string> doctrines; // This is a vector in order to keep order consistent. We want the first things read (tenets) to be the first things output, ALWAYS
	private KeyValuePair<long, Religion?> religion;
};