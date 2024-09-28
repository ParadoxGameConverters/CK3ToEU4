using System;
using System.Collections.Generic;
using commonItems;
using commonItems.Colors;

namespace CK3ToEU4.CK3.CoatsOfArms;

public class CoatOfArms
{
    public CoatOfArms(BufferedReader reader, long theId, ColorFactory colorFactory)
    {
        Id = theId;
        
        var parser = new Parser();
        RegisterKeys(parser, colorFactory);
        parser.ParseStream(reader);
    }

    private void RegisterKeys(Parser parser, ColorFactory colorFactory)
    {
        parser.RegisterKeyword("pattern", reader => Pattern = reader.GetString());
        parser.RegisterKeyword("color1", reader =>
        {
            try
            {
                Color1 = colorFactory.GetColor(reader);
            }
            catch (Exception e)
            {
                Logger.Warn(e.Message + " - sidestepping with black.");
                Color1 = new Color(0, 0, 0);
            }
        });
        parser.RegisterKeyword("color2", reader =>
        {
            try
            {
                Color2 = colorFactory.GetColor(reader);
            }
            catch (Exception e)
            {
                Logger.Warn(e.Message + " - sidestepping with black.");
                Color2 = new Color(0, 0, 0);
            }
        });
        parser.RegisterKeyword("color3", reader =>
        {
            try
            {
                Color3 = colorFactory.GetColor(reader);
            }
            catch (Exception e)
            {
                Logger.Warn(e.Message + " - sidestepping with black.");
                Color3 = new Color(0, 0, 0);
            }
        });
        parser.RegisterKeyword("textured_emblem", reader => texturedEmblems.Add(new Emblem(reader, colorFactory)));
        parser.RegisterKeyword("colored_emblem", reader => coloredEmblems.Add(new Emblem(reader, colorFactory)));
        parser.RegisterKeyword("sub", reader => { subs.Add(new CoatOfArms(reader, 0, colorFactory)); });
        parser.RegisterKeyword("instance", reader =>
        {
            var instance = new EmblemInstance(reader);
            if (instance.Offset.Count == 0)
                instance.DefaultOffset();
            instances.Add(instance);
        });
        parser.RegisterKeyword("parent", reader => { Parent = new(reader.GetString(), null); });
        parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
    }

    public long Id { get; private set; } = 0;
    public string? Pattern { get; private set; }
    public Color? Color1 { get; private set; } // mask: 255, 0, 0 - red
    public Color? Color2 { get; private set; } // mask: 255, 255, 0 - yellow
    public Color? Color3 { get; private set; } // mask: 255, 255, 255 - white
    public IReadOnlyList<Emblem> TexturedEmblems => texturedEmblems;
    private readonly List<Emblem> texturedEmblems = [];
    public IReadOnlyList<Emblem> ColoredEmblems => coloredEmblems;
    private readonly List<Emblem> coloredEmblems = [];
    public IReadOnlyList<CoatOfArms> Subs => subs; // These are sub-coatsofarms. They are recursive!
    private readonly List<CoatOfArms> subs = [];
    public IReadOnlyList<EmblemInstance> Instances => instances; // Used to position sub-coats.
    private readonly List<EmblemInstance> instances = [];
    public KeyValuePair<string, CoatOfArms?>? Parent { get; private set; } // Used for recursive external flags.
}