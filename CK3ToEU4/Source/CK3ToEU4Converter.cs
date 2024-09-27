using System;
using commonItems;

namespace CK3ToEU4.NET.Source; // TODO: modify the project to make this just "CK3ToEU4" instead of "CK3ToEU4.NET.Source"

public static class CK3ToEU4Converter
{
    public static void ConvertCK3ToEU4(ConverterVersion converterVersion)
    {
        Logger.Progress(0);
        var theConfiguration = new Configuration.Config(converterVersion);
        throw new NotImplementedException();
        // TODO: CK3World sourceWorld = new(theConfiguration, converterVersion);
        // TODO: EU4::World destWorld(sourceWorld, *theConfiguration, converterVersion);
        Logger.Notice("* Conversion complete *");
        Logger.Progress(100);
    }

}