using System;
using System.IO;
using commonItems;

namespace CK3ToEU4;

public static class Program
{
    public static int Main(string[] args)
    {
        try {
            // Clear the log.txt file.
            using var clearLog = new StreamWriter("log.txt");
            clearLog.Close();


            ConverterVersion converterVersion = new();
            converterVersion.LoadVersion("configurables/version.txt");
            Logger.Info(converterVersion.ToString());
            if (args.Length >= 2)
            {
                Logger.Info("CK3ToEU4 takes no parameters.");
                Logger.Info("It uses configuration.txt, configured manually or by the frontend.");
            }
            CK3ToEU4Converter.ConvertCK3ToEU4(converterVersion);
            return 0;
        }

        catch (Exception e)
        {
            Logger.Error(e.ToString());
            return -1;
        }
    }
}