using System;
using CK3ToEU4.Configuration;
using commonItems;

using CK3World = CK3ToEU4.CK3.World;

namespace CK3ToEU4;


public static class CK3ToEU4Converter
{
    public static void ConvertCK3ToEU4(ConverterVersion converterVersion)
    {
        Logger.Progress(0);
        var theConfiguration = new Config(converterVersion);
        CK3World sourceWorld = new(theConfiguration, converterVersion);
        throw new NotImplementedException();
        // TODO: EU4::World destWorld(sourceWorld, *theConfiguration, converterVersion);
        Logger.Notice("* Conversion complete *");
        Logger.Progress(100);
    }

}