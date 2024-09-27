using System;
using System.IO;
using commonItems;
using CommonFunctions = commonItems.CommonFunctions;

namespace CK3ToEU4.NET.Source.Configuration;

public enum StartDate
{
    EU = 1,
    CK = 2,
    Manual = 3,
}

public enum SplitVassals
{
    Yes = 1,
    No = 2
}
public enum DeJure
{
    Enabled = 1,
    Disabled = 2
}
public enum Development
{
    Import = 1,
    Vanilla = 2
}
public enum MultiProvDevTransfer
{
    Default = 1,
    MP = 2
}
public enum Sunset
{
    Disabled = 1,
    Active = 2
}
public enum IAmHRE
{
    HRE = 1,
    Byzantium = 2,
    Rome = 3,
    Custom = 4,
    None = 5
}
public enum ShatterEmpires
{
    None = 1,
    All = 2,
    Custom = 3
}
public enum ShatterLevel
{
    Duchy = 1,
    Kingdom = 2
}
public enum ShatterHRELevel
{
    Duchy = 1,
    Kingdom = 2
}
public enum Siberia
{
    SmallSiberia = 1,
    ClearSiberia = 2,
    LeaveSiberia = 3
}
public enum Institutions
{
    Historic = 1,
    Dynamic = 2
}
public enum DynasticNames
{
    Enabled = 1,
    Disabled = 2
}
public enum DiscoveredBy
{
    Vanilla = 1,
    Dynamic = 2
}

public class Config
{
    public string SaveGamePath { get; private set; } = string.Empty;
    public string CK3Path { get; private set; } = string.Empty;
    public string CK3DocPath { get; private set; } = string.Empty;
    public string EU4Path { get; private set; } = string.Empty;
    public string OutputName { get; private set; } = string.Empty;
    public StartDate StartDateOption { get; private set; } = StartDate.EU;
    public IAmHRE HRE { get; private set; } = IAmHRE.HRE;
    public ShatterEmpires ShatterEmpires { get; private set; } = ShatterEmpires.None;
    public ShatterHRELevel ShatterHRELevel { get; private set; } = ShatterHRELevel.Duchy;
    public ShatterLevel ShatterLevel { get; private set; } = ShatterLevel.Duchy;
    public Siberia Siberia { get; private set; } = Siberia.ClearSiberia;
    public Sunset Sunset { get; private set; } = Sunset.Disabled;
    public Institutions DynamicInstitutions { get; private set; } = Institutions.Historic;
    public Development Development { get; private set; } = Development.Import;
    public MultiProvDevTransfer MultiProvinceDevelopmentTransfer { get; private set; } = MultiProvDevTransfer.Default;
    public DeJure DeJure { get; private set; } = DeJure.Enabled;
    public SplitVassals SplitVassals { get; private set; } = SplitVassals.Yes;
    public DynasticNames DynasticNames { get; private set; } = DynasticNames.Enabled;
    public DiscoveredBy DiscoveredBy { get; private set; } = DiscoveredBy.Vanilla;
    
    public Config(ConverterVersion converterVersion)
    {
        Logger.Info("Reading configuration file");
        var parser = new Parser();
        RegisterKeys(parser);
        parser.ParseFile("configuration.txt");

        SetOutputName();
        VerifyCK3Path();
        VerifyCK3Version(converterVersion);
        VerifyEU4Path();
        VerifyEU4Version(converterVersion);
        Logger.Progress(3);
    }

    public Config(BufferedReader reader)
    {
        var parser = new Parser();
        RegisterKeys(parser);
        parser.ParseStream(reader);

        SetOutputName();
    }

    private void RegisterKeys(Parser parser)
    {
        parser.RegisterKeyword("SaveGame", reader =>
        {
            SaveGamePath = reader.GetString();
            Logger.Info($"Save Game set to: {SaveGamePath}");
        });
        parser.RegisterKeyword("CK3directory", reader => CK3Path = reader.GetString());
        parser.RegisterKeyword("EU4directory", reader => EU4Path = reader.GetString());
        parser.RegisterKeyword("CK3DocDirectory", reader => CK3DocPath = reader.GetString());
        parser.RegisterKeyword("output_name", reader => {
            OutputName = reader.GetString();
            Logger.Info($"Output name set to: {OutputName}");
        });
        parser.RegisterKeyword("start_date", reader =>
        {
            var startDateString = reader.GetString();
            StartDateOption = (StartDate)int.Parse(startDateString);
            Logger.Info($"Start date set to: {startDateString}");
        });
        parser.RegisterKeyword("i_am_hre", reader => {
            var hreString = reader.GetString();
            HRE = (IAmHRE)int.Parse(hreString);
            Logger.Info($"HRE set to: {hreString}");
        });
        parser.RegisterKeyword("dejure", reader => {
            var deJureString = reader.GetString();
            DeJure = (DeJure)int.Parse(deJureString);
            Logger.Info($"DeJure set to: {deJureString}");
        });
        parser.RegisterKeyword("split_vassals", reader => {
            var splitVassalsString = reader.GetString();
            SplitVassals = (SplitVassals)int.Parse(splitVassalsString);
            Logger.Info($"Split Vassals set to: {splitVassalsString}");
        });
        parser.RegisterKeyword("shatter_empires", reader => {
            var shatterEmpiresString = reader.GetString();
            ShatterEmpires = (ShatterEmpires)int.Parse(shatterEmpiresString);
            Logger.Info($"Shatter Empires set to: {shatterEmpiresString}");
        });
        parser.RegisterKeyword("shatter_level", reader => {
            var shatterLevelString = reader.GetString();
            ShatterLevel = (ShatterLevel)int.Parse(shatterLevelString);
            Logger.Info($"Shatter Level set to: {shatterLevelString}");
        });
        parser.RegisterKeyword("shatter_hre_level", reader => {
            var shatterHRELevelString = reader.GetString();
            ShatterHRELevel = (ShatterHRELevel)int.Parse(shatterHRELevelString);
            Logger.Info($"Shatter HRE Level set to: {shatterHRELevelString}");
        });
        parser.RegisterKeyword("siberia", reader => {
            var siberiaString = reader.GetString();
            Siberia = (Siberia)int.Parse(siberiaString);
            Logger.Info($"Siberia set to: {siberiaString}");
        });
        parser.RegisterKeyword("sunset", reader => {
            var sunsetString = reader.GetString();
            Sunset = (Sunset)int.Parse(sunsetString);
            Logger.Info($"Sunset set to: {sunsetString}");
        });
        parser.RegisterKeyword("dynamicInstitutions", reader => {
            var dynamicInstitutionsString = reader.GetString();
            DynamicInstitutions = (Institutions)int.Parse(dynamicInstitutionsString);
            Logger.Info($"Dynamic Institutions set to: {dynamicInstitutionsString}");
        });
        parser.RegisterKeyword("development", reader => {
            var developmentString = reader.GetString();
            Development = (Development)int.Parse(developmentString);
            Logger.Info($"Development set to: {developmentString}");
        });
        parser.RegisterKeyword("multiprovdevtransfer", reader => {
            var multiProvDevTransferString = reader.GetString();
            MultiProvinceDevelopmentTransfer = (MultiProvDevTransfer)int.Parse(multiProvDevTransferString);
            Logger.Info($"Multiple Province Dev Transfer set to: {multiProvDevTransferString}");
        });
        parser.RegisterKeyword("dynasticNames", reader => {
            var dynasticNamesString = reader.GetString();
            DynasticNames = (DynasticNames)int.Parse(dynasticNamesString);
            Logger.Info($"Dynastic Names set to: {dynasticNamesString}");
        });
        parser.RegisterKeyword("discoveredBy", reader => {
            var discoveredByString = reader.GetString();
            DiscoveredBy = (DiscoveredBy)int.Parse(discoveredByString);
            Logger.Info($"Discovered By set to: {discoveredByString}");
        });
        parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
    }

    private void VerifyCK3Path()
    {
        if (!Directory.Exists(CK3Path))
            throw new Exception(CK3Path + " does not exist!");
        // TODO: OSX and Linux paths are speculative
        if (!File.Exists(CK3Path + "/binaries/ck3.exe") &&
            !File.Exists(CK3Path + "/CK3game") &&
            !File.Exists(CK3Path + "/binaries/ck3"))
            throw new Exception(CK3Path + " does not contain Crusader Kings 3!");
        if (!File.Exists(CK3Path + "/game/map_data/positions.txt"))
            throw new Exception(CK3Path + " does not appear to be a valid CK3 install!");
        Logger.Info($"\tCK3 install path is {CK3Path}");
        CK3Path += "/game/"; // We're adding "/game/" since all we ever need from now on is in that subdirectory.
    }

    private void VerifyEU4Path()
    {
        if (!Directory.Exists(EU4Path))
            throw new Exception(EU4Path + " does not exist!");
        if (!File.Exists(EU4Path + "/eu4.exe") && !File.Exists(EU4Path + "/eu4"))
            throw new Exception(EU4Path + " does not contain Europa Universalis 4!");
        if (!File.Exists(EU4Path + "/map/positions.txt"))
            throw new Exception(EU4Path + " does not appear to be a valid EU4 install!");
        Logger.Info($"\tEU4 install path is {EU4Path}");
    }

    private void SetOutputName()
    {
        if (string.IsNullOrEmpty(OutputName))
        {
            OutputName = CommonFunctions.TrimPath(SaveGamePath);
        }

        OutputName = CommonFunctions.TrimExtension(OutputName);
        OutputName = CommonFunctions.ReplaceCharacter(OutputName, '-');
        OutputName = CommonFunctions.ReplaceCharacter(OutputName, ' ');

        OutputName = CommonFunctions.NormalizeUTF8Path(OutputName);
        Logger.Info($"Using output name {OutputName}");
    }

    private void VerifyCK3Version(ConverterVersion converterVersion)
    {
        var ck3Version = GameVersion.ExtractVersionFromLauncher(CK3Path + "../launcher/launcher-settings.json");
        if (ck3Version is null)
        {
            Logger.Error("CK3 version could not be determined, proceeding blind!");
            return;
        }

        Logger.Info($"CK3 version: {ck3Version.ToShortString()}");

        if (converterVersion.MinSource > ck3Version)
        {
            Logger.Error($"CK3 version is v{ck3Version.ToShortString()}" +
                ", converter requires minimum v" +
                $"{converterVersion.MinSource.ToShortString()}!");
            throw new Exception("Converter vs CK3 installation mismatch!");
        }

        if (!converterVersion.MaxSource.IsLargerishThan(ck3Version))
        {
            Logger.Error($"CK3 version is v{ck3Version.ToShortString()}" +
                ", converter requires maximum v" +
                $"{converterVersion.MaxSource.ToShortString()}!");
            throw new Exception("Converter vs CK3 installation mismatch!");
        }
    }

    private void VerifyEU4Version(ConverterVersion converterVersion)
    {
        var eu4Version = GameVersion.ExtractVersionFromLauncher(EU4Path + "/launcher-settings.json");
        if (eu4Version is null)
        {
            Logger.Error("EU4 version could not be determined, proceeding blind!");
            return;
        }

        Logger.Info($"EU4 version: {eu4Version.ToShortString()}");

        if (converterVersion.MinTarget > eu4Version)
        {
            Logger.Error($"EU4 version is v{eu4Version.ToShortString()}" +
                ", converter requires minimum v" +
                $"{converterVersion.MinTarget.ToShortString()}!");
            throw new Exception("Converter vs EU4 installation mismatch!");
        }

        if (!converterVersion.MaxTarget.IsLargerishThan(eu4Version))
        {
            Logger.Error($"EU4 version is v{eu4Version.ToShortString()}" +
                ", converter requires maximum v" +
                $"{converterVersion.MaxTarget.ToShortString()}!");
            throw new Exception("Converter vs EU4 installation mismatch!");
        }
    }
}