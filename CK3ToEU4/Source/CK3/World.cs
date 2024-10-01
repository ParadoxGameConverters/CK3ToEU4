using System;
using System.Collections.Generic;
using System.IO;
using CK3ToEU4.CK3.Titles;
using CK3ToEU4.Configuration;
using commonItems;
using commonItems.Mods;
using SystemUtils = commonItems.SystemUtils;

namespace CK3ToEU4.CK3;

using Mods = List<Mod>;

struct SaveData
{
    internal string gameState;
    internal string metadata; // We use this to set up mods before main processing.
    internal bool parsedMeta = false;

    public SaveData()
    {
	    gameState = null;
	    metadata = null;
    }
}

public class World
{
    public Date ConversionDate { get; set; } = "1444.11.11"; // A.K.A. the end date
    public Dictionary<string, Title> IndependentTitles { get; }
    public Mods Mods { get; private set; } = new();
    public Titles Titles { get; }
    public Faiths Faiths { get; } = new();
    public Title? HRETitle { get; private set; }
    public bool IslamExists { get; private set; }
    public bool CoADesignerEnabled { get; private set; }
    public string? PlayerTitleId { get; private set; }
    public LocalizationMapper LocalizationMapper { get; }
    public CultureMapper CultureMapper { get; }
    public Cultures Cultures { get; }

    public World(Config theConfiguration, ConverterVersion converterVersion)
    {
        var parser = new Parser();
        RegisterKeys(parser, theConfiguration, converterVersion);
        Logger.Progress(4);

        Logger.Info("-> Verifying CK3 save.");
        verifySave(theConfiguration.SaveGamePath);
        processSave(theConfiguration.SaveGamePath);
        Logger.Progress(5);

        parser.ParseStream(new BufferedReader(saveGame.metadata));
        Logger.Progress(10);

        Logger.Info("* Priming Converter Components *");
        primeLaFabricaDeColor(theConfiguration);
        loadLandedTitles(theConfiguration);
        loadCharacterTraits(theConfiguration);
        loadHouseNames(theConfiguration);
        Logger.Progress(15);
        // Scraping localizations from CK3 so we may know proper names for our countries and people.
        Logger.Info("-> Reading Words");
        LocalizationMapper.scrapeLocalizations(theConfiguration, Mods);
        CultureMapper.loadCulturesFromDisk();

        Logger.Info("* Parsing Gamestate *");
        parser.ParseStream(new BufferedReader(saveGame.gameState));
        Logger.Progress(20);

        Logger.Info("* Gamestate Parsing Complete, Weaving Internals *");
        crosslinkDatabases();
        Logger.Progress(30);

        // processing
        Logger.Info("-- Checking For Religions");
        checkForIslam();
        Logger.Info("-- Flagging HRE Provinces");
        flagHREProvinces(theConfiguration);
        Logger.Info("-- Shattering HRE");
        shatterHRE(theConfiguration);
        Logger.Info("-- Shattering Empires");
        shatterEmpires(theConfiguration);
        Logger.Info("-- Filtering Independent Titles");
        filterIndependentTitles();
        Logger.Info("-- Splitting Off Vassals");
        splitVassals(theConfiguration);
        Logger.Info("-- Rounding Up Some People");
        gatherCourtierNames();
        Logger.Info("-- Congregating DeFacto Counties for Independent Titles");
        congregateDFCounties();
        Logger.Info("-- Congregating DeJure Counties for Independent Titles");
        congregateDJCounties();
        Logger.Info("-- Filtering Landless Titles");
        filterLandlessTitles();
        Logger.Info("-- Distributing Electorates");
        setElectors();

        if (CoADesignerEnabled && playerId is not null)
        {
            Logger.Info("-- Locating Player Title.");
            locatePlayerTitle(theConfiguration);
        }

        Logger.Info("*** Good-bye CK3, rest in peace. ***");
        Logger.Progress(47);
    }

    private void RegisterKeys(Parser parser, Config theConfiguration, ConverterVersion converterVersion)
    {
        Logger.Info("*** Hello CK3, Deus Vult! ***");
        metaPreParser.RegisterRegex("SAV.*", reader => { });
        metaPreParser.RegisterKeyword("meta_data", reader =>
        {
            metaParser.ParseStream(reader);
            saveGame.parsedMeta = true;
        });
        metaPreParser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);

        metaParser.RegisterKeyword("mods", reader =>
        {
            Logger.Info("-> Detecting used mods.");
            HashSet<string> seenMods = [];
            foreach (var path in reader.GetStrings())
            {
                if (seenMods.Contains(path))
                    continue;
                Mods.Add(new Mod("", path));
                seenMods.Add(path);
            }

            Logger.Info($"<> Savegame claims {Mods.Count} mods used.");
            ModLoader modLoader = new();
            modLoader.LoadMods(theConfiguration.CK3DocPath, Mods);
            Mods = modLoader.UsableMods;
            foreach (var mod in Mods)
                if (mod.Name == "CoA Designer")
                {
                    Logger.Notice("CoA Designer mod enabled; player CoA will be generated.");
                    CoADesignerEnabled = true;
                }
        });
        metaParser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);

        parser.RegisterRegex("SAV.*", reader => { });
        parser.RegisterKeyword("meta_data", reader =>
        {
            if (saveGame.parsedMeta)
            {
                ParserHelpers.IgnoreItem(reader);
            }
            else
            {
                metaParser.ParseStream(reader);
                saveGame.parsedMeta = true;
            }
        });
        parser.RegisterKeyword("currently_played_characters", reader =>
        {
	        var playedCharacters = reader.GetLongs();
            if (playedCharacters.Count > 0)
                playerId = playedCharacters[0];
        });
        parser.RegisterKeyword("date", reader => {
            ConversionDate = reader.GetString();
        });
        parser.RegisterKeyword("bookmark_date", reader => {
            startDate = reader.GetString();
        });
        parser.RegisterKeyword("version", reader =>
        {
	        string versionString = reader.GetString();
            ck3Version = new GameVersion(versionString);
            Logger.Info($"<> Savegame version: {versionString}");

            if (converterVersion.MinSource > ck3Version)
            {
                Logger.Error("Converter requires a minimum save from v" +
                    converterVersion.MinSource.ToShortString());
                throw new Exception("Savegame vs converter version mismatch!");
            }

            if (!converterVersion.MaxSource.IsLargerishThan(ck3Version))
            {
	            Logger.Error("Converter requires a maximum save from v" +
	                         converterVersion.MaxSource.ToShortString());
                throw new Exception("Savegame vs converter version mismatch!");
            }
        });
        parser.RegisterKeyword("variables", reader =>
        {
	        Logger.Info("-> Loading variable flags.");
            flags = Flags(reader);
            Logger.Info("<> Loaded " << flags.getFlags().size() << " variable flags.");
        });
        parser.RegisterKeyword("landed_titles", reader =>
        {
	        Logger.Info("-> Loading titles.");
            Titles = new Titles(reader);
            var counter = Titles.getCounter();
            Logger.Info("<> Loaded " << Titles.getTitles().size() << " titles: " << counter[0] << "b " <<
                counter[1] << "c " << counter[2] << "d "
                << counter[3] << "k " << counter[4] << "e, " << counter[5] << "dynamics.");
        });
        parser.RegisterKeyword("provinces", reader =>
        {
	        Logger.Info("-> Loading provinces.");
            provinceHoldings = ProvinceHoldings(theStream);
            Logger.Info("<> Loaded " << provinceHoldings.getProvinceHoldings().size() << " provinces.");
        });
        parser.RegisterKeyword("living", reader =>
        {
            Logger.Info("-> Loading potentially alive human beings.");
            characters.loadCharacters(theStream);
            Logger.Info("<> Loaded " << characters.getCharacters().size() << " human entities.");
        });
        parser.RegisterKeyword("dead_unprunable", reader => {
            Logger.Info("-> Loading dead people.");
            characters.loadCharacters(theStream);
            Logger.Info("<> Loaded " << characters.getCharacters().size() << " human remains.");
        });
        parser.RegisterKeyword("dynasties", reader => {
            Logger.Info("-> Loading dynasties.");
            dynasties = Dynasties(reader);
            houses = dynasties
                .getHouses(); // Do not access houses in dynasties after this - there are none and will crash.
            Logger.Info("<> Loaded " << dynasties.getDynasties().size() << " dynasties and " <<
                houses.getHouses().size() << " houses.");
        });
        parser.RegisterKeyword("religion", reader => {
            Logger.Info("-> Loading religions.");
            religions = Religions(reader);
            faiths = religions
                .getFaiths(); // Do not access faiths in religions after this - there are none and will crash.
            Logger.Info("<> Loaded " << religions.getReligions().size() << " religions and " <<
                faiths.getFaiths().size() << " faiths.");
        });
        parser.RegisterKeyword("coat_of_arms", reader => {
            Logger.Info("-> Loading garments of limbs.");
            coats = CoatsOfArms(theStream);
            Logger.Info("<> Loaded " << coats.getCoats().size() << " wearables.");
        });
        parser.RegisterKeyword("county_manager", reader =>
        {
	        Logger.Info("-> Loading county details.");
            countyDetails = CountyDetails(reader);
            Logger.Info("<> Loaded " << countyDetails.getCountyDetails().size() << " county details.");
        });
        parser.RegisterKeyword("culture_manager", reader => {
            Logger.Info("-> Loading cultures.");
            cultures = Cultures(reader);
            Logger.Info("<> Loaded " << Cultures.getCultures().size() << " cultures.");
        });
        parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
    }

    private void locatePlayerTitle(Config theConfiguration)
    {
	    foreach (var (titleId, title) in IndependentTitles)
	    {
		    if (title.getHolder() && title.getHolder().first == playerId)
		    {
			    Logger.Info("Player title: " << title.getName();
			    PlayerTitleId = title.first;
			    break;
		    }
	    }
        

        if (PlayerTitleId)
        {
            theConfiguration.setCraftFlagForPlayerTitle(PlayerTitleId);
        }
    }
    
    
private void processSave(string saveGamePath)
{
	std::ifstream saveFile(fs::u8path(saveGamePath), std::ios::binary);
	stringstream inStream;
	inStream << saveFile.rdbuf();
	saveGame.gamestate = inStream.str();

	const auto save = rakaly::parseCk3(saveGame.gamestate);
	if (const auto& melt = save.meltMeta(); melt)
	{
		Logger.Info("Meta extracted successfully.");
		melt->writeData(saveGame.metadata);
	}
	else if (save.is_binary())
	{
		Log(LogLevel::Error) << "Binary Save and NO META!");
	}

	if (save.is_binary())
	{
		Logger.Info("Gamestate is binary, melting.");
		const auto& melt = save.melt();
		if (melt.has_unknown_tokens())
		{
			Log(LogLevel::Error) << "Rakaly reports errors while melting ironman save!");
		}

		melt.writeData(saveGame.gamestate);
	}
	else
	{
		Logger.Info("Gamestate is textual.");
		const auto& melt = save.melt();
		melt.writeData(saveGame.gamestate);
	}

	// Always dump to disk for easier debug.
	std::ofstream metaDump("metaDump.txt");
	metaDump << saveGame.metadata;
	metaDump.close();

	std::ofstream saveDump("saveDump.txt");
	saveDump << saveGame.gamestate;
	saveDump.close();
}

private void verifySave(const string& saveGamePath)
{
	std::ifstream saveFile(fs::u8path(saveGamePath), std::ios::binary);
	if (!saveFile.is_open())
		throw std::runtime_error("Could not open save! Exiting!");

	char buffer[10];
	saveFile.get(buffer, 4);
	if (buffer[0] != 'S' || buffer[1] != 'A' || buffer[2] != 'V')
		throw std::runtime_error("Savefile of unknown type.");

	saveFile.close();
}

private void primeLaFabricaDeColor(Config theConfiguration)
{
	Logger.Info("-> Loading colors.");
	foreach (var file: commonItems::GetAllFilesInFolder(theConfiguration.getCK3Path() + "common/named_colors"))
	{
		if (file.find(".txt") == string::npos)
			continue;
		namedColors.loadColors(theConfiguration.getCK3Path() + "common/named_colors/" + file);
	}
	foreach (var mod: mods)
	{
		if (!commonItems::DoesFolderExist(mod.path + "common/named_colors"))
			continue;
		Logger.Info("<> Loading some colors from [" << mod.name << "]");
		foreach (var file: commonItems::GetAllFilesInFolder(mod.path + "common/named_colors"))
		{
			if (file.find(".txt") == string::npos)
				continue;
			namedColors.loadColors(mod.path + "common/named_colors/" + file);
		}
	}
	Logger.Info("<> Loaded " << laFabricaDeColor.getRegisteredColors().size() << " colors.");
}

private void loadLandedTitles(Config theConfiguration)
{
	Logger.Info("-> Loading Landed Titles.");
	ModFilesystem modFS = new(theConfiguration.CK3Path, mods);
	foreach (var file: modFS.GetAllFilesInFolder("common/landed_titles/"))
	{
		if (getExtension(file) != "txt")
			continue;
		landedTitles.loadTitles(file);
	}
	Logger.Info("<> Loaded " << landedTitles.getFoundTitles().size() << " landed titles.");
}

private void loadCharacterTraits(Config theConfiguration)
{
	Logger.Info("-> Examiming Personalities");
	foreach (var file: commonItems::GetAllFilesInFolder(theConfiguration.getCK3Path() + "common/traits"))
	{
		if (file.find(".txt") == string::npos)
			continue;
		traitScraper.loadTraits(theConfiguration.getCK3Path() + "common/traits/" + file);
	}
	foreach (var mod: mods)
	{
		if (!commonItems::DoesFolderExist(mod.path + "common/traits"))
			continue;
		Logger.Info("<> Loading some character traits from [" << mod.name << "]");
		foreach (var file: commonItems::GetAllFilesInFolder(mod.path + "common/traits"))
		{
			if (file.find(".txt") == string::npos)
				continue;
			traitScraper.loadTraits(mod.path + "common/traits/" + file);
		}
	}
	Logger.Info(">> " << traitScraper.getTraits().size() << " personalities scrutinized.");
}

private void loadHouseNames(Config theConfiguration)
{
	Logger.Info("-> Loading House Names");
	foreach (var file in SystemUtils.GetAllFilesInFolder(theConfiguration.CK3Path + "common/dynasty_houses"))
	{
		if (CommonFunctions.GetExtension(file) != "txt")
			continue;
		houseNameScraper.loadHouseDetails(theConfiguration.CK3Path + "common/dynasty_houses/" + file);
	}
	foreach (var mod in Mods)
	{
		if (!Directory.Exists(mod.Path + "common/dynasty_houses"))
			continue;
		Logger.Info("<> Loading house names from [" + mod.Name + "]");
		foreach (var  file in SystemUtils.GetAllFilesInFolder(mod.Path + "common/dynasty_houses"))
		{
			if (CommonFunctions.GetExtension(file) != "txt")
				continue;
			houseNameScraper.loadHouseDetails(mod.Path + "common/dynasty_houses/" + file);
		}
	}
	Logger.Info(">> " << houseNameScraper.getHouseNames().size() << " house names read.");
}

private void crosslinkDatabases()
{
	Logger.Info("-> Injecting Names into Houses.");
	houses.importNames(houseNameScraper);

	Logger.Info("-> Concocting Cultures.");
	Cultures.concoctCultures(LocalizationMapper, CultureMapper);

	HashSet<Culture> cultureSet = [];
	foreach (var culture in Cultures.getCultures().Values)
		cultureSet.Add(culture);
	CultureMapper.storeCultures(cultureSet);
	Logger.Info("-> Loading Cultures into Counties.");
	countyDetails.linkCultures(Cultures);
	Logger.Info("-> Loading Cultures into Characters.");
	characters.linkCultures(Cultures);
	Logger.Info("-> Loading Faiths into Counties.");
	countyDetails.linkFaiths(faiths);
	Logger.Info("-> Loading Faiths into Characters.");
	characters.linkFaiths(faiths);
	Logger.Info("-> Loading Faiths into Religions.");
	religions.linkFaiths(faiths);
	Logger.Info("-> Loading Religions into Faiths.");
	faiths.linkReligions(religions, Titles);
	Logger.Info("-> Loading Coats into Coats.");
	coats.linkParents(Titles);
	Logger.Info("-> Loading Coats into Dynasties.");
	dynasties.linkCoats(coats);
	Logger.Info("-> Loading Coats into Titles.");
	Titles.linkCoats(coats);
	Logger.Info("-> Loading Holdings into Clay.");
	landedTitles.linkProvinceHoldings(provinceHoldings);
	Logger.Info("-> Loading Counties into Clay.");
	landedTitles.linkCountyDetails(countyDetails);
	Logger.Info("-> Loading Dynasties into Houses.");
	houses.linkDynasties(dynasties);
	Logger.Info("-> Loading Characters into Houses.");
	houses.linkCharacters(characters);
	Logger.Info("-> Loading Houses into Characters.");
	characters.linkHouses(houses);
	Logger.Info("-> Loading Characters into Titles.");
	Titles.linkCharacters(characters);
	Logger.Info("-> Loading Titles into Characters.");
	characters.linkTitles(Titles);
	Logger.Info("-> Loading Titles into Titles.");
	Titles.linkTitles();
	Logger.Info("-> Fixing Titles Pointing To Wrong Places.");
	Titles.relinkDeFactoVassals();
	Logger.Info("-> Loading Titles into Clay.");
	landedTitles.linkTitles(Titles);
	Logger.Info("-> Loading Characters into Characters.");
	characters.linkCharacters();
	Logger.Info("-> Loading Clay into Titles.");
	Titles.linkLandedTitles(landedTitles);
	Logger.Info("-> Loading Traits into Characters.");
	characters.linkTraits(traitScraper);
}

private void flagHREProvinces(Config theConfiguration)
{
	string hreTitleStr;
	switch (theConfiguration.getHRE())
	{
		case Configuration::I_AM_HRE::HRE:
			hreTitleStr = "e_hre";
			break;
		case Configuration::I_AM_HRE::BYZANTIUM:
			hreTitleStr = "e_byzantium";
			break;
		case Configuration::I_AM_HRE::ROME:
			hreTitleStr = "e_roman_empire";
			break;
		case Configuration::I_AM_HRE::CUSTOM:
			hreTitleStr = iAmHreMapper.getHRE();
			break;
		case Configuration::I_AM_HRE::NONE:
			Logger.Info(">< HRE Provinces not available due to configuration disabling HRE Mechanics.");
			return;
	}
	const auto& allTitles = titles.getTitles();
	const auto& theHre = allTitles.find(hreTitleStr);
	if (theHre == allTitles.end())
	{
		Logger.Info(">< HRE Provinces not available, " << hreTitleStr << " not found!");
		return;
	}
	if (theHre->second->getDFVassals().empty())
	{
		Logger.Info(">< HRE Provinces not available, " << hreTitleStr << " has no vassals!");
		return;
	}
	if (!theHre->second->getHolder())
	{
		Logger.Info(">< HRE Provinces not available, " << hreTitleStr << " has no holder!");
		return;
	}

	// store for later.
	hreTitle = std::make_pair(hreTitleStr, theHre->second);

	const auto counter = theHre->second->flagDeJureHREProvinces();
	Logger.Info("<> " << counter << " HRE provinces flagged.");
}

private void checkForIslam()
{
	foreach (var county: countyDetails.getCountyDetails() | std::views::values)
	{
		if (!county->getFaith().second)
			continue;
		if (!county->getFaith().second->getReligion().second)
			continue;
		if (county->getFaith().second->getReligion().second->getName() == "islam_religion")
		{
			islamExists = true;
			return;
		}
	}
}

private void shatterHRE(Config theConfiguration) const
{
	if (!hreTitle)
		return;
	const auto& hreHolder = hreTitle->second->getHolder();
	Logger.Info("HRE Holder: " << hreHolder->second->getName();
	bool emperorSet = false; // "Emperor", in this context, is not a person but the resulting primary duchy/kingdom title of said person.
	Dictionary<long, Character?> brickedPeople; // these are people we need to fix.

	// First we are composing a list of all HRE members. These are duchies,
	// so we're also ripping them from under any potential kingdoms.
	Dictionary<long, Title?> hreMembers;
	Dictionary<long, Title?> brickList;
	foreach (var vassal: hreTitle->second->getDFVassals())
	{
		if (vassal.second->getLevel() == LEVEL::DUCHY || vassal.second->getLevel() == LEVEL::COUNTY)
		{
			hreMembers.insert(vassal);
		}
		else if (vassal.second->getLevel() == LEVEL::KINGDOM)
		{
			if (vassal.second->getName() == "k_papal_state" || vassal.second->getName() == "k_orthodox" ||
				 theConfiguration.getShatterHRELevel() == Configuration::SHATTER_HRE_LEVEL::KINGDOM) // hard override for special HRE members
			{
				hreMembers.insert(vassal);
			}
			else
			{
				foreach (var vassalvassal: vassal.second->getDFVassals())
				{
					hreMembers.insert(vassalvassal);
				}
				// Bricking the kingdom.
				brickedPeople.insert(*vassal.second->getHolder());
				brickList.insert(vassal);
			}
		}
		else if (vassal.second->getLevel() != LEVEL::BARONY)
		{
			Log(LogLevel::Warning) << "Unrecognized HRE vassal: " << vassal.first << " - " << vassal.second->getName();
		}
	}

	foreach (var brick: brickList)
		brick.second->brickTitle();

	// Locating HRE emperor. Unlike CK2, we'll using first non-hreTitle non-landless title from hreHolder's domain.
	if (!hreHolder->second->getCharacterDomain())
		throw std::runtime_error("HREmperor has no Character Domain!");

	foreach (var hreHolderTitle: hreHolder->second->getCharacterDomain()->getDomain())
	{
		if (hreHolderTitle.second->getName() == hreTitle->first) // this is what we're breaking, ignore it.
			continue;
		if (hreHolderTitle.second->getLevel() == LEVEL::BARONY) // Absolutely ignore baronies.
			continue;
		if (hreHolderTitle.second->getLevel() == LEVEL::KINGDOM && theConfiguration.getShatterHRELevel() == Configuration::SHATTER_HRE_LEVEL::DUTCHY)
			continue; // This is bricked.
		if (hreHolderTitle.second->getClay() && !hreHolderTitle.second->getClay()->isLandless())
		{ // looks solid.
			hreHolderTitle.second->setHREEmperor();
			Logger.Info("Flagging " << hreHolderTitle.second->getName() << " as His HREship.");
			emperorSet = true;
			break;
		}
	}

	if (!emperorSet)
		Log(LogLevel::Warning) << "Couldn't flag His HREship as emperor does not own any viable titles!");

	// We're flagging hre members as such, as well as setting them free.
	foreach (var member: hreMembers)
	{
		member.second->setInHRE();
		member.second->grantIndependence(); // This fill free emperor's holdings as well. We'll reintegrate them immediately after this.
	}

	// Finally we brick the hre.
	brickedPeople.insert(*hreHolder);
	hreTitle->second->brickTitle();

	// Exception to ripping are some members that are (still) in hreHolder's domain. These would be some titles he had that were not bricked but went
	// independent; If hreHolder has empire+duchy+county, county may not go free and needs to go back under the duchy. Now that we've cleaned up bricked title(s)
	// from his domain, we can fix the loose ones.

	foreach (var afflictedPerson: brickedPeople)
	{
		const auto& holderDomain = afflictedPerson.second->getCharacterDomain()->getDomain();
		const auto holderTitles = Dictionary(holderDomain.begin(), holderDomain.end());

		foreach (var holderTitle: holderDomain)
		{
			// does this title have a DJLiege that is was in his domain, and survived bricking, but does not have DFLiege since it was granted independence?
			if (!holderTitle.second->getDFLiege() && holderTitle.second->getDJLiege() && holderTitle.second->getDJLiege()->second->getHolder() &&
				 holderTitle.second->getDJLiege()->second->getHolder()->first == afflictedPerson.first && holderTitles.count(holderTitle.first))
			{
				// fix this title.
				const auto& djLiege = holderTitle.second->getDJLiege();
				djLiege->second->addDFVassals(Dictionary{holderTitle});
				holderTitle.second->loadDFLiege(*djLiege);
			}
		}
	}

	Logger.Info("<> " << hreMembers.size() << " HRE members released.");
}

private void shatterEmpires(Config theConfiguration) const
{
	if (theConfiguration.getShatterEmpires() == Configuration::SHATTER_EMPIRES::NONE)
	{
		Logger.Info(">< Empire shattering disabled by configuration.");
		return;
	}

	bool shatterKingdoms = true; // the default.
	switch (theConfiguration.getShatterLevel())
	{
		case Configuration::SHATTER_LEVEL::KINGDOM:
			shatterKingdoms = false;
			break;
		case Configuration::SHATTER_LEVEL::DUTCHY:
			shatterKingdoms = true;
			break;
	}
	const auto& allTitles = titles.getTitles();

	foreach (var empire: allTitles)
	{
		if (hreTitle && empire.first == hreTitle->first)
			continue; // This is HRE, wrong function for that one.
		if (theConfiguration.getShatterEmpires() == Configuration::SHATTER_EMPIRES::CUSTOM && !shatterEmpiresMapper.isEmpireShatterable(empire.first))
			continue; // Only considering those listed.
		if (empire.second->getLevel() != LEVEL::EMPIRE && theConfiguration.getShatterEmpires() != Configuration::SHATTER_EMPIRES::CUSTOM)
			continue; // Otherwise only empires.
		if (empire.second->getDFVassals().empty())
			continue; // Not relevant.
		if (!empire.second->getHolder())
			continue; // No holder.

		Dictionary<long, Character?> brickedPeople; // these are people we need to fix.
		// First we are composing a list of all members.
		Dictionary<long, Title?> members;
		Dictionary<long, Title?> brickList;
		foreach (var vassal: empire.second->getDFVassals())
		{
			if (!vassal.second)
			{
				Log(LogLevel::Warning) << "Shattering vassal " << vassal.first << " that isn't linked! Skipping!");
				continue;
			}
			if (vassal.second->getLevel() == LEVEL::DUCHY || vassal.second->getLevel() == LEVEL::COUNTY)
			{
				members.insert(vassal);
			}
			else if (vassal.second->getLevel() == LEVEL::KINGDOM)
			{
				if (shatterKingdoms && vassal.second->getName() != "k_papal_state" && vassal.second->getName() != "k_orthodox")
				{ // hard override for special empire members

					foreach (var vassalVassal: vassal.second->getDFVassals())
					{
						if (!vassalVassal.second)
							Log(LogLevel::Warning) << "VassalVassal " << vassalVassal.first << " has no link!");
						else
							members.insert(vassalVassal);
					}
					// Bricking the kingdom
					if (!vassal.second->getHolder()->second)
					{
						Log(LogLevel::Warning) << "Vassal " << vassal.second->getName() << " has no holder linked!");
					}
					else
					{
						brickedPeople.insert(*vassal.second->getHolder());
					}
					brickList.insert(vassal);
				}
				else
				{
					// Not shattering kingdoms.
					members.insert(vassal);
				}
			}
			else if (vassal.second->getLevel() != LEVEL::BARONY)
			{
				Log(LogLevel::Warning) << "Unrecognized vassal level: " << vassal.first;
			}
		}

		foreach (var brick: brickList)
			brick.second->brickTitle();

		// grant independence to ex-vassals.
		foreach (var member: members)
		{
			member.second->grantIndependence();
		}

		// Finally, dispose of the shell.
		brickedPeople.insert(*empire.second->getHolder());
		empire.second->brickTitle();

		// Same as with HREmperor, we need to roll back counties or duchies that got released from ex-emperor himself or kings.
		foreach (var afflictedPerson: brickedPeople)
		{
			if (!afflictedPerson.second)
			{
				Log(LogLevel::Warning) << "Character " << afflictedPerson.first << " has no link! Cannot fix them.");
				continue;
			}
			else if (!afflictedPerson.second->getCharacterDomain())
			{
				Log(LogLevel::Warning) << "Character " << afflictedPerson.first << " has no link to domain! Cannot fix them.");
				continue;
			}
			else if (afflictedPerson.second->getCharacterDomain()->getDomain().empty())
			{
				continue;
			}

			const auto& holderDomain = afflictedPerson.second->getCharacterDomain()->getDomain();
			const auto holderTitles = Dictionary(holderDomain.begin(), holderDomain.end());

			foreach (var holderTitle: holderDomain)
			{
				// does this title have a DJLiege that is was in his domain, and survived bricking, but does not have DFLiege since it was granted independence?
				if (!holderTitle.second->getDFLiege() && holderTitle.second->getDJLiege() && holderTitle.second->getDJLiege()->second->getHolder() &&
					 holderTitle.second->getDJLiege()->second->getHolder()->first == afflictedPerson.first && holderTitles.count(holderTitle.first))
				{
					// fix this title.
					const auto& djLiege = holderTitle.second->getDJLiege();
					djLiege->second->addDFVassals(Dictionary{holderTitle});
					holderTitle.second->loadDFLiege(*djLiege);
				}
			}
		}

		Logger.Info("<> " << empire.first << " shattered, " << members.size() << " members released.");
	}
}

private void filterIndependentTitles()
{
	const auto& allTitles = titles.getTitles();
	Dictionary<string, Title?> potentialIndeps;

	foreach (var title: allTitles)
	{
		if (!title.second->getHolder())
			continue; // don't bother with titles without holders.
		if (!title.second->getDFLiege())
		{
			// this is a potential indep.
			potentialIndeps.insert(title);
		}
		if (title.second->getDFLiege() && !title.second->getDFLiege()->second->getHolder()) // yes, we can have a dfliege that's destroyed, apparently.
		{
			// this is also potential indep.
			potentialIndeps.insert(title);
			// And do fix it.
			title.second->grantIndependence();
		}
	}

	// Check if the holder holds any actual land (b|c_something). (Only necessary for the holder,
	// no need to recurse, we're just filtering landless titular titles like mercenaries
	// or landless Pope. If a character holds a landless titular title along actual title
	// (like Caliphate), it's not relevant at this stage as he's independent anyway.

	// First, split off all county_title holders into a container.
	HashSet<long> countyHolders;
	Dictionary<long, Dictionary<string, Title?>> allTitleHolders;
	foreach (var title: allTitles)
	{
		if (title.second->getHolder())
		{
			if (title.second->getLevel() == LEVEL::COUNTY)
				countyHolders.insert(title.second->getHolder()->first);
			allTitleHolders[title.second->getHolder()->first].insert(title);
		}
	}

	// Then look at all potential indeps and see if their holders hold physical clay.
	auto counter = 0;
	foreach (var indep: potentialIndeps)
	{
		const auto& holderID = indep.second->getHolder()->first;
		if (countyHolders.count(holderID))
		{
			// this fellow holds a county, so his indep title is an actual title.
			independentTitles.insert(indep);
			counter++;
			// Set The Pope
			if (indep.first == "k_papal_state")
			{
				indep.second->setThePope();
				Logger.Info("---> " << indep.first << " is the Pope.");
			}
			else
			{
				if (allTitleHolders[holderID].count("k_papal_state"))
				{
					indep.second->setThePope();
					Logger.Info("---> " << indep.first << " belongs to the Pope.");
				}
			}
		}
	}
	Logger.Info("<> " << counter << " independent titles recognized.");
}

private void splitVassals(Config theConfiguration)
{
	if (theConfiguration.getSplitVassals() == Configuration::SPLITVASSALS::NO)
	{
		Logger.Info(">< Splitting vassals disabled by configuration.");
		return;
	}

	Dictionary<string, Title?> newIndeps;

	// We know who's independent. We can go through all indeps and see what should be an independent vassal.
	foreach (var title: independentTitles)
	{
		if (title.second->isThePope())
			continue; // Not touching the pope.
		// let's not split hordes or tribals. <- TODO: Add horde here once some DLC drops.
		if (title.second->getHolder()->second->getCharacterDomain()->getGovernment() == "tribal_government")
			continue;
		auto relevantVassals = 0;
		LEVEL relevantVassalLevel;
		if (title.second->getLevel() == LEVEL::EMPIRE)
			relevantVassalLevel = LEVEL::KINGDOM;
		else if (title.second->getLevel() == LEVEL::KINGDOM)
			relevantVassalLevel = LEVEL::DUCHY;
		else
			continue; // Not splitting off counties.
		foreach (var vassal: title.second->getDFVassals())
		{
			if (vassal.second->getLevel() != relevantVassalLevel)
				continue; // they are not relevant
			if (vassal.second->coalesceDFCounties().empty())
				continue; // no land, not relevant
			relevantVassals++;
		}
		if (!relevantVassals)
			continue;																		// no need to split off anything.
		const auto& countiesClaimed = title.second->coalesceDFCounties(); // this is our primary total.
		foreach (var vassal: title.second->getDFVassals())
		{
			if (vassal.second->getLevel() != relevantVassalLevel)
				continue; // they are not relevant
			if (vassal.second->getHolder()->first == title.second->getHolder()->first)
				continue; // Not splitting our own land.
			const auto& vassalProvincesClaimed = vassal.second->coalesceDFCounties();

			// a vassal goes indep if they control 1/relevantvassals + 10% land.
			double threshold = static_cast<double>(countiesClaimed.size()) / relevantVassals + 0.1 * static_cast<double>(countiesClaimed.size());
			threshold *= vassalSplitoffMapper.getFactor();
			if (static_cast<double>(vassalProvincesClaimed.size()) > threshold)
				newIndeps.insert(KeyValuePair(vassal.second->getName(), vassal.second));
		}
	}

	// Now let's free them.
	foreach (var newIndep: newIndeps)
	{
		const auto& liege = newIndep.second->getDFLiege();
		liege->second->addGeneratedVassal(newIndep);
		newIndep.second->loadGeneratedLiege(KeyValuePair(liege->second->getName(), liege->second));
		newIndep.second->grantIndependence();
		independentTitles.insert(newIndep);
	}
	Logger.Info("<> " << newIndeps.size() << " vassals liberated from immediate integration.");
}

private void gatherCourtierNames()
{
	// We're using this function to locate courtiers, assemble their names as potential Monarch Names in EU4,
	// and also while at it, to see if they hold adviser jobs. It's anything but trivial, as being employed doesn't equate with
	// being a councilor, nor do landed councilors have employers if they work for their liege.

	auto counter = 0;
	auto counterAdvisors = 0;
	Dictionary<long, Dictionary<string, bool>> holderCourtiers;								// holder-name/male
	Dictionary<long, Dictionary<long, Character?>> holderCouncilors; // holder-councilors

	foreach (var character: characters.getCharacters())
	{
		// Do you even exist?
		if (!character.second)
			continue;
		// Hello. Are you an employed individual?
		if (!character.second->isCouncilor() && !character.second->getEmployer())
			continue;
		// If you have a steady job, we need your employer's references.
		if (character.second->isCouncilor())
		{
			if (character.second->getEmployer() && character.second->getEmployer()->second)
			{
				// easiest case.
				holderCourtiers[character.second->getEmployer()->first].insert(KeyValuePair(character.second->getName(), !character.second->isFemale()));
				holderCouncilors[character.second->getEmployer()->first].insert(character);
			}
			else if (character.second->getCharacterDomain() && !character.second->getCharacterDomain()->getDomain().empty())
			{
				// this councilor is landed and works for his liege.
				const auto& characterPrimaryTitle = character.second->getCharacterDomain()->getDomain()[0];
				if (!characterPrimaryTitle.second)
					continue; // corruption
				const auto& liegeTitle = characterPrimaryTitle.second->getDFLiege();
				if (!liegeTitle || liegeTitle->second)
					continue; // I dislike this character. I think it is time he was let go.
				const auto& liege = liegeTitle->second->getHolder();
				if (!liege || !liege->second)
					continue; // Or maybe we should fire his liege.
				holderCourtiers[liege->first].insert(KeyValuePair(character.second->getName(), character.second->isFemale()));
				holderCouncilors[liege->first].insert(character);
			}
			else
			{
				// Doesn't have employer and doesn't have land but is councilor. Bollocks.
				continue;
			}
		}
		else if (character.second->getEmployer())
		{
			// Being employed but without a council task means a knight or physician or similar. Works for us.
			holderCourtiers[character.second->getEmployer()->first].insert(KeyValuePair(character.second->getName(), !character.second->isFemale()));
		}
	}

	// We're only interested in those working for indeps.
	foreach (var title: independentTitles)
	{
		const auto containerItr = holderCourtiers.find(title.second->getHolder()->first);
		if (containerItr != holderCourtiers.end())
		{
			title.second->getHolder()->second->loadCourtierNames(containerItr->second);
			counter += static_cast<int>(containerItr->second.size());
		}
		const auto councilorItr = holderCouncilors.find(title.second->getHolder()->first);
		if (councilorItr != holderCouncilors.end())
		{
			title.second->getHolder()->second->loadCouncilors(councilorItr->second);
			counterAdvisors += static_cast<int>(councilorItr->second.size());
		}
	}
	Logger.Info("<> " << counter << " people gathered for interrogation. " << counterAdvisors << " were detained.");
}

private void congregateDFCounties()
{
	auto counter = 0;
	// We're linking all contained counties for a title's tree under that title.
	// This will form actual EU4 tag and contained provinces.
	foreach (var title: independentTitles)
	{
		title.second->congregateDFCounties();
		foreach (var province: title.second->getOwnedDFCounties())
		{
			province.second->loadHoldingTitle(KeyValuePair(title.first, title.second));
		}
		counter += static_cast<int>(title.second->getOwnedDFCounties().size());
	}
	Logger.Info("<> " << counter << " counties held by independents.");
}

private void congregateDJCounties()
{
	int counter = 0;
	// We're linking all dejure provinces under the title as these will be the base
	// for that title's permanent claims, unless already owned.
	foreach (var title: independentTitles)
	{
		title.second->congregateDJCounties();
		counter += static_cast<int>(title.second->getOwnedDJCounties().size());
	}
	Logger.Info("<> " << counter << " de jure provinces claimed by independents.");
}

private void filterLandlessTitles()
{
	int counter = 0;
	HashSet<string> titlesForDisposal;
	foreach (var title in IndependentTitles)
	{
		if (title.second->getOwnedDFCounties().empty())
		{
			titlesForDisposal.insert(title.first);
		}
	}
	foreach (var drop in titlesForDisposal)
	{
		IndependentTitles.Remove(drop);
		counter++;
	}
	Logger.Info($"<> {counter} empty titles dropped, {IndependentTitles.Count} remain.");
}

private void setElectors()
{
	// Finding electorates is not entirely trivial. CK3 has 7-XX slots, one of which is usually the Emperor himself, but
	// he is not considered an elector in EU4 sense unless he holds one of the electorate titles which are historical.
	// However, CK3 doesn't care about titles, it stores people, so a multiduke with a secondary electoral title will still
	// be elector and we need to flag his primary title as electorate one, as other duchies will possibly be annexed or PU-d.
	// Moreover, these electors may not even be indeps after HRE shattering as player may opt to keep kingdoms but electors were
	// under these kings. We can't help that.

	if (!HRETitle)
	{
		Logger.Info(">< HRE does not exist.");
		return;
	}
	var electors = HRETitle->getElectors();
	if (electors.empty())
	{
		Logger.Info(">< HRE does not have electors.");
		return;
	}

	int counter = 0;

	// Preambule done, we start here.
	// Make a registry of indep titles and their holders.
	Dictionary<long, Dictionary<string, Title?>> holderTitles; // holder/titles
	KeyValuePair<long, Character?> hreHolder;

	foreach (var title: independentTitles)
	{
		holderTitles[title.second->getHolder()->first].insert(title);
		if (title.second->isHREEmperor())
		{
			hreHolder = *title.second->getHolder();
		}
	}

	if (!hreHolder.first)
	{
		Logger.Info(">< HRE does not have an emperor.");
		return;
	}

	// Now roll through electors and flag their primary titles as electors. If kings get electorate status
	// but kingdoms are also shattered, tough luck? Their primary duchy won't inherit electorate as they could
	// end up with multiple electorates, and that's possible only through EU4 gameplay and causes massive
	// penalties to IA.

	foreach (var elector in electors)
	{
		if (counter >= 7)
			break; // We had enough.

		if (electors.size() > 7 && elector.first == hreHolder.first)
		{
			continue; // We're skipping the emperor for 8+ slot setups.
		}

		// How many indep titles does he hold? If any?
		const auto& regItr = holderTitles.find(elector.first);
		if (regItr == holderTitles.end())
		{
			continue; // This fellow was cheated out of electorate titles.
		}

		// Which title is his primary? The first one in his domain (that survived the shattering)
		if (elector.second->getCharacterDomain() && !elector.second->getCharacterDomain()->getDomain().empty())
		{
			foreach (var electorTitle: elector.second->getCharacterDomain()->getDomain())
			{
				// mark this title as electorate if it's independent and has land.
				if (regItr->second.count(electorTitle.second->getName()) && !electorTitle.second->getOwnedDFCounties().empty())
				{
					electorTitle.second->setElectorate();
					Log(LogLevel::Debug) << "Setting electorate: " << electorTitle.second->getName();
					counter++;
					break;
				}
			}
			// If we marked none here, then all his titles are dependent and he's not a good elector choice.
		}
		else
		{
			// This is a fellow without a domain? Mark some independent non-landless title as electorate.
			foreach (var title: regItr->second)
			{
				if (!title.second->getOwnedDFCounties().empty())
				{
					title.second->setElectorate();
					Log(LogLevel::Debug) << "Setting electorate: " << title.first;
					counter++;
					break;
				}
			}
			// otherwise no helping this fellow.
		}
	}

	Logger.Info("<> " << counter << " electorates linked.");
}


    private Parser metaParser = new();
    private Parser metaPreParser = new();

    private Date startDate = "1.1.1";
    private long? playerId = null;
    private GameVersion ck3Version;
    private ProvinceHoldings provinceHoldings;
    private Characters characters;
    private Dynasties dynasties;
    private Houses houses;
    private Religions religions;
    private Faiths faiths;
    private CoatsOfArms coats;
    private LandedTitles landedTitles;
    private Flags flags;
    private CountyDetails countyDetails;
    private HouseNameScraper houseNameScraper;
    private NamedColors namedColors;
    private IAmHreMapper iAmHreMapper;
    private ShatterEmpiresMapper shatterEmpiresMapper;
    private TraitScraper traitScraper;
    private VassalSplitoffMapper vassalSplitoffMapper;

    private SaveData saveGame;
}