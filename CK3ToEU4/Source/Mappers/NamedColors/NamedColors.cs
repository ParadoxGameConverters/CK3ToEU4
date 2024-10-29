using commonItems;
using commonItems.Colors;

namespace CK3ToEU4.Mappers.NamedColors;

class NamedColors
{
	public NamedColors() {}
	public void loadColors(BufferedReader reader, ColorFactory colorFactory)
	{
		var parser = new Parser();
		RegisterKeys(parser, colorFactory);
		parser.ParseStream(reader);
	}
	public void loadColors(string filepath, ColorFactory colorFactory)
	{
		var parser = new Parser();
		RegisterKeys(parser, colorFactory);
		parser.ParseFile(filepath);
	}

	private static void RegisterKeys(Parser parser, ColorFactory colorFactory)
	{
		// If we get a color named "colors", we're f--d.
		parser.RegisterKeyword("colors", reader => {
			var loadedColors = new NamedColors();
			loadedColors.loadColors(reader, colorFactory);
		});
		parser.RegisterRegex(CommonRegexes.Catchall, (reader, colorName) => {
			colorFactory.AddNamedColor(colorName, reader);
		});
	}
};