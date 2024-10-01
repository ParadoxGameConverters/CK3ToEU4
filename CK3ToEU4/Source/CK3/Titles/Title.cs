using System;
using System.Collections.Generic;
using CK3ToEU4.CK3.CoatsOfArms;
using CK3ToEU4.Configuration;
using commonItems;
using commonItems.Colors;

namespace CK3ToEU4.CK3.Titles;

enum Level
{
    Barony,
    County,
    Duchy,
    Kingdom,
    Empire
} 

public class Title
{
	public long Id { get; } = 0; // 11038
	public bool isTheocraticLease() { return theocraticLease; }
	public bool isCountyCapitalBarony() { return cCapitalBarony; }
	public bool isDuchyCapitalBarony() { return dCapitalBarony; }
	public bool isHREEmperor() { return HREEmperor; }
	public bool isInHRE() { return inHRE; }
	public bool isThePope() { return thePope; }
	public bool isElectorate() { return electorate; }
	public bool isHolderCapital() { return holderCapital; }
	public bool isHRECapital() { return HRECapital; }
	public bool isCustomTitle() { return customTitle; }
	public bool isRenamed() { return renamed; }
	public bool isManualNameClaimed() { return nameClaimed; }
	public string getName() { return name; }
	public string getDisplayName() { return displayName; }
	public string getAdjective() { return adjective; }
	public Date getCreationDate() { return creationDate; }
	public string getHistoryGovernment() { return historyGovernment; }
	public KeyValuePair<long, Title> getCapital() { return capital; }
	public KeyValuePair<long, Title>? getDFLiege() { return dfLiege; }
	public KeyValuePair<long, Title> getDJLiege() { return djLiege; }
	public Dictionary<long, Title> getDFVassals() { return dfVassals; }
	public Dictionary<long, Title> getDJVassals() { return djVassals; }
	public List<KeyValuePair<long, Character>> getHeirs() { return heirs; }
	public Dictionary<long, Character> getClaimants() { return claimants; }
	public Dictionary<long, Character?> getElectors() { return electors; }
	public HashSet<string> getLaws() { return laws; }
	public KeyValuePair<long, Character>? getHolder() { return holder; }
	public KeyValuePair<long, CoatOfArms>? getCoA() { return coa; }
	public LandedTitles getClay() { return clay; }
	public Dictionary<string, Title> getOwnedDFCounties() { return ownedDFCounties; } // TODO: change return type
	public Dictionary<string, Title> getOwnedDJCounties() { return ownedDJCounties; }
	public KeyValuePair<string, Title> getHoldingTitle() { return holdingTitle; }
	public KeyValuePair<string, Title>? getGeneratedLiege() { return generatedLiege; }
	public Dictionary<string, Title> getGeneratedVassals() { return generatedVassals; }
	public KeyValuePair<string, Country>? getEU4Tag() { return tagCountry; }
	public List<KeyValuePair<long, Character>> getPreviousHolders() { return previousHolders; }

	public Color? getColor();
	public bool isLandless();
	public bool areDFLiegeAndDJLiegeSet();
	public bool areDFLiegeAndDJLiegeSame();
	public bool isHolderSet() { return holder != std::nullopt; }
	public bool isHolderLinked() { return holder  holder->second; }
	public bool doesHolderHaveCharacterDomain();
	public HashSet<string> getTitleNamesFromHolderDomain();
	public Level getLevel();

	public Title(BufferedReader reader, long theID)
	{
		Id = theID;

		var parser = new Parser();
		RegisterKeys(parser);
		parser.ParseStream(reader);
		
		// Obligatory sanity check.
		if (!holder)
		{
			// This title does NOT in fact exist.
			dfLiege = null;
		}
	}

	#region linkage
	public void loadCoat(KeyValuePair<long, CoatOfArms> coat) { coa = coat; }
	public void loadDFLiege(KeyValuePair<long, Title> DFLiege) { dfLiege = DFLiege; }
	public void loadDJLiege(KeyValuePair<long, Title> DJLiege) { djLiege = DJLiege; }
	public void loadDFVassals(Dictionary<long, Title> DFVassals) { dfVassals = DFVassals; }
	public void addDFVassals(Dictionary<long, Title> DFVassals) { dfVassals.insert(DFVassals.begin(), DFVassals.end()); }
	public void loadDJVassals(Dictionary<long, Title> DJVassals) { djVassals = DJVassals; }
	public void loadHolder(KeyValuePair<long, Character> theHolder) { holder = theHolder; }
	public void loadHeirs(List<KeyValuePair<long, Character>> theHeirs) { heirs = theHeirs; }
	public void loadPreviousHolders(List<KeyValuePair<long, Character>> theHolders) { previousHolders = theHolders; }
	public void loadClaimants(Dictionary<long, Character> theClaimants) { claimants = theClaimants; }
	public void loadElectors(Dictionary<long, Character> theElectors) { electors = theElectors; }
	public void loadClay(LandedTitles theClay) { clay = theClay; }
	public void loadOwnedDFCounties( Dictionary<string, Title> theOwnedCounties) { ownedDFCounties = theOwnedCounties; }
	public void loadOwnedDJCounties( Dictionary<string, Title> theOwnedCounties) { ownedDJCounties = theOwnedCounties; }
	#endregion

	#region processing
	public void setDynamicLevel(Level theLevel) { dynamicLevel = theLevel; }
	public void cleanUpDisplayName();
	public int flagDeJureHREProvinces();
	public void brickTitle();
	public void grantIndependence();
	public void resetDFLiege() { dfLiege.reset(); }
	public void resetDJLiege() { djLiege.reset(); }
	public void setHREEmperor() { HREEmperor = true; }
	public void setInHRE() { inHRE = true; }
	public void dropTitleFromDFVassals(long long titleID);
	public void setThePope() { thePope = true; }
	public void setCustomTitle() { customTitle = true; }
	public void setManualNameClaim() { nameClaimed = true; }
	public void pickDisplayName(const Dictionary<string, Title?>& possibleTitles); // Grants one county's name to another during N:1/N:M mappings
	public Title findDuchyCapital();																 // Only for c_, for now
	public void congregateDFCounties();
	public void congregateDJCounties();
	public void loadGeneratedLiege(const KeyValuePair<string, Title?>& liege) { generatedLiege = liege; }
	public void addGeneratedVassal(const KeyValuePair<string, Title?>& theVassal) { generatedVassals.insert(theVassal); }
	public void loadHoldingTitle(const KeyValuePair<string, Title?>& theTitle) { holdingTitle = theTitle; }
	public void setElectorate() { electorate = true; }
	public void relinkDeFactoVassals();
	#endregion

	#region conversion
	public void loadEU4Tag(const KeyValuePair<string, std::shared_ptr<EU4::Country>>& theCountry) { tagCountry = theCountry; }
	public double getBuildingWeight(const mappers::DevWeightsMapper& devWeightsMapper) const;
	public void setHolderCapital() { holderCapital = true; }
	public void setHRECapital() { HRECapital = true; }
	public void clearGeneratedVassals() { generatedVassals.clear(); }

	Dictionary<string, Title> coalesceDFCounties() const;
	Dictionary<string, Title> coalesceDJCounties() const;
	#endregion

	private void RegisterKeys(Parser parser, ColorFactory colorFactory)
	{
			parser.RegisterKeyword("key", reader => {
		name = reader.GetString();
	});
	parser.RegisterKeyword("name", reader => {
		displayName = reader.GetString();
		if (displayName.find("\x15") != string::npos)
		{
			cleanUpDisplayName();
		}
	});
	parser.RegisterKeyword("adj", reader => {
		adjective = reader.GetString();
	});
	parser.RegisterKeyword("date", reader => {
		creationDate = reader.GetString();
	});
	parser.RegisterKeyword("claim", reader => {
		foreach (var claimantID in reader.GetLongs())
			claimants.Add(claimantID, null);
	});
	parser.RegisterKeyword("history_government", reader => {
		historyGovernment = reader.GetString();
	});
	parser.RegisterKeyword("theocratic_lease", reader => {
		theocraticLease = reader.GetString() == "yes";
	});
	parser.RegisterKeyword("capital_barony", reader => {
		cCapitalBarony = reader.GetString() == "yes";
	});
	parser.RegisterKeyword("duchy_capital_barony", reader => {
		dCapitalBarony = reader.GetString() == "yes";
	});
	parser.RegisterKeyword("capital", reader => {
		capital = new(reader.GetLong(), null);
	});
	parser.RegisterKeyword("de_facto_liege", reader => {
		dfLiege = KeyValuePair(commonItems::singleLlong(theStream).getLlong(), nullptr);
	});
	parser.RegisterKeyword("de_jure_liege", reader => {
		djLiege = KeyValuePair(commonItems::singleLlong(theStream).getLlong(), nullptr);
	});
	parser.RegisterKeyword("de_jure_vassals", reader => {
		for (auto vassalID: commonItems::llongList(theStream).getLlongs())
			djVassals.insert(std::make_pair(vassalID, nullptr));
	});
	parser.RegisterKeyword("heir", reader => {
		for (auto heirID: commonItems::llongList(theStream).getLlongs())
			heirs.emplace_back(std::make_pair(heirID, nullptr));
	});
	parser.RegisterKeyword("laws", reader => {
		const auto& theLaws = commonItems::stringList(theStream).getStrings();
		laws = std::set(theLaws.begin(), theLaws.end());
	});
	parser.RegisterKeyword("holder", reader => {
		holder = KeyValuePair(commonItems::singleLlong(theStream).getLlong(), nullptr);
	});
	parser.RegisterKeyword("renamed", [this](std::istream& theStream) {
		renamed = commonItems::getString(theStream) == "yes";
	});
	parser.RegisterKeyword("coat_of_arms_id", reader => {
		coa = new KeyValuePair<long, CoatOfArms?>(reader.GetLong(), null);
	});
	parser.RegisterKeyword("succession_election", reader => {
		const auto newTitle = Title(theStream, 0);
		electors = newTitle.getElectors();
	});
	parser.RegisterKeyword("electors", reader => {
		foreach (var electorID in reader.GetInts())
			electors.Add(new KeyValuePair<long, Character>(electorID, null));
	});
	parser.RegisterKeyword("landless", reader => {
		landless = reader.GetString() == "yes";
	});
	parser.RegisterKeyword("color", reader => {
		color = colorFactory.GetColor(reader);
	});
	parser.RegisterKeyword("history", reader => {
		previousHolders = new Title(reader, 0).getPreviousHolders();
	});
	parser.RegisterRegex(CommonRegexes.Date, reader =>
	{
		var questionableItem = reader.GetStringOfItem().ToString();
		var tempReader = new BufferedReader(questionableItem);
		if (!questionableItem.Contains('{'))
		{
			try
			{
				previousHolders.Add(new(tempReader.GetLong(), null));
			}
			catch (Exception e)
			{
				Logger.Warn($"Invalid previous holder ID: {questionableItem}");
			}
		}
	});
	parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}


    																			
    private KeyValuePair<long, Title?> capital;							// capital title is a COUNTY, even for county itself and baronies beneath it!
    private string name;																			// c_ashmaka
    private string displayName;																// Ashmaka
    private string adjective;																	// Ashmakan
    private string article;																		// "the ". Not always present.
    private string historyGovernment;														// Unclear why this is history. Maybe further governments override it.
    private Date creationDate;																		// Unclear. Ranges to 9999.1.1, probably is PDX alternative for "bool isCreated";
    private KeyValuePair<long, CoatOfArms?>? coa;	// This is dejure flag but not defacto.
    private KeyValuePair<long, Title>? dfLiege;		// defacto liege title (d_kalyani)
    private KeyValuePair<long, Title>? djLiege;		// dejure liege title (d_rattapadi)
    private KeyValuePair<long, Character>? holder; // Holding character
    private Dictionary<long, Title> dfVassals;						// defacto vassals, not in save, manually linked post-loading
    private Dictionary<long, Title> djVassals;						// dejure vassals (for all except baronies and titulars)
    private List<KeyValuePair<long, Character>> heirs;		// Order of heirs is unclear so we're keeping them ordered and using first if able.
    private Dictionary<long, Character?> claimants;					// People holding a claim to this title. Incredibly useful.
    private Dictionary<long, Character> electors;					// People involved in elections regardless of election type law.
    private bool theocraticLease = false;															// Does this apply to non-baronies? Maybe? Who owns it then, dejure liege?
    private HashSet<string> laws;
    private bool cCapitalBarony = false;
    private bool dCapitalBarony = false;
	private LandedTitles clay; // Middleware towards geographical data, essential for b_c_, potentially obsolete for others.
	private bool HREEmperor = false;
	private bool inHRE = false;
	private bool thePope = false;
	private bool customTitle = false;													// True if title was fromed via "Found a New Kingdom/Empire" decision. Vestigal
	private bool renamed = false;														// True if title was manually named
	private bool nameClaimed = false;													// Flag for 1:M (or N:M) province mappings to decide who gets the manual name
	private Dictionary<string, Title> ownedDFCounties; // used to map higher-lvl titles directly to clay. Includes self! Every c_+ title has this.
	private Dictionary<string, Title> ownedDJCounties; // ditto
	private KeyValuePair<string, Title>? generatedLiege; // Liege we set manually while splitting vassals.
	private Dictionary<string, Title> generatedVassals;					// Vassals we split off deliberately.
	private KeyValuePair<string, Title> holdingTitle;						// topmost owner title (e_francia or similar), only c_s have this.
	private bool electorate = false;
	private KeyValuePair<string, EU4.Country>? tagCountry;
	private bool landless = false;
	private Color? color;
	private List<KeyValuePair<long, Character?>> previousHolders;
	private bool holderCapital = false;
	private bool HRECapital = false;
	private Level? dynamicLevel; // Maybe assigned through dynamic ranks, otherwise getLevel will try to guesstimate.

}