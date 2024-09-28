using System;
using System.Collections.Generic;
using commonItems;
using commonItems.Colors;

namespace CK3ToEU4.CK3.CoatsOfArms;

public class Emblem
{
    public Emblem(BufferedReader reader, ColorFactory colorFactory)
    {
        var parser = new Parser();
        RegisterKeys(parser, colorFactory);
        parser.ParseStream(reader);
    }

    private void RegisterKeys(Parser parser, ColorFactory colorFactory)
    {
        parser.RegisterKeyword("texture", reader => {
            Texture = reader.GetString();
        });
        parser.RegisterKeyword("color1", reader => {
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
        parser.RegisterKeyword("color2", reader => {
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
        parser.RegisterKeyword("color3", reader => {
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
        parser.RegisterKeyword("mask", reader =>
        {
            mask = reader.GetInts();
        });
        parser.RegisterKeyword("instance", reader => {
            var instance = new EmblemInstance(reader);
            if (instance.Position.Count == 0)
                instance.DefaultPosition();
            instances.Add(instance);
        });
       parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
    }
    
    public string? Texture { get; private set; }
    public Color? Color1 { get; private set; }
    public Color? Color2 { get; private set; }
    public Color? Color3 { get; private set; }
    public IReadOnlyList<int> Mask => mask;
    private List<int> mask = [];
    public IReadOnlyList<EmblemInstance> Instances => instances;
    private readonly List<EmblemInstance> instances = [];
}