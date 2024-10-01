using System;
using System.Collections.Generic;
using commonItems;
using commonItems.Colors;

namespace CK3ToEU4.CK3.Titles;

class LandedTitles
{
	// This is a recursive class that scrapes 00_landed_titles.txt (and related files) looking for title colors, landlessness,
	// and most importantly relation between baronies and barony provinces so we can link titles to actual clay.
	// Since titles are nested according to hierarchy we do this recursively.

	// Keep in mind that we use this class as middleware between titles and hard geographical data. Baronies and Counties have said data,
	// but newfangled custom empires and such found in Titles will not be present here. They should have colors defined in their Title block
	// anyway, and whatever relates to Title over there takes precedence over data in this class.

	public void loadTitles(BufferedReader reader)
	{
		var parser = new Parser();
		registerKeys(parser);
		parser.ParseStream(reader);
	}
	public void loadTitles(string fileName)
	{
		var parser = new Parser();
		registerKeys(parser);
		parser.ParseFile(fileName);
	}

	public bool DefiniteForm { get;private set; } = false;
	public bool Landless { get; private set; } = false;
	public bool CanBeNamedAfterDynasty { get; private set; } = true;
	public Color? Color { get; private set; }
	
	// This is of questionable use as savegame already defines defacto capitals. Not always present, and if present then a COUNTY.
	public KeyValuePair<string, Title?>? Capital { get; private set; }
	
	// only b_baronies have these - holdings are related to individual provinces on map.
	public KeyValuePair<int, ProvinceHolding?>? Province { get; private set; }
	
	// only c_counties have these - these define common data for group of baronies under county.
	public KeyValuePair<string, CountyDetail?>? County { get; private set; }

	// We're using title name, not savegame ID for key value.
	public IReadOnlyDictionary<string, LandedTitles?> FoundTitles => foundTitles;
	private readonly Dictionary<string, LandedTitles?> foundTitles;

	public void loadProvinceHolding(const KeyValuePair<int, ProvinceHolding?>& provinceHolding) { province = provinceHolding; }
	public void loadCountyDetails(const KeyValuePair<string, std::shared_ptr<CountyDetail>>& countyDetail) { county = countyDetail; }
	public void loadCapital(const KeyValuePair<string, Title?>& theCapital) { capital = theCapital; }

	public void linkProvinceHoldings(ProvinceHoldings provinceHoldings) {
		// We're linking provinces into BARONY titles only, as other titles have no holding data.
		int counter = 0;
		var provinceData = provinceHoldings.getProvinceHoldings();
		foreach (var landedTitle in foundTitles)
		{
			if (landedTitle.first.find("b_") != 0)
				continue;
			if (!landedTitle.second->getProvince())
				throw std::runtime_error("Landed title " + landedTitle.first + " has not province holding defined!");

			const auto& provinceDataItr = provinceData.find(landedTitle.second->getProvince()->first);
			if (provinceDataItr != provinceData.end())
			{
				landedTitle.second->loadProvinceHolding(*provinceDataItr);
				++counter;
			}
			else
			{
				throw new Exception("Landed title " + landedTitle.first + " has province holding " + std::to_string(landedTitle.second->getProvince()->first) +
				                    " which has no definition!");
			}
		}
		Logger.Info("<> " + counter + " landed titles updated.");
	}
	public void linkCountyDetails(CountyDetails countyDetails) {
		// We're linking county details into COUNTY titles only, as other titles have no such details.
		int counter = 0;
		var countyData = countyDetails.getCountyDetails();
		foreach (var landedTitle in foundTitles)
		{
			if (!landedTitle.Key.StartsWith("c_"))
				continue;
			const auto& countyDataItr = countyData.find(landedTitle.first);
			if (countyDataItr != countyData.end())
			{
				landedTitle.second->loadCountyDetails(*countyDataItr);
				++counter;
			}
			else
			{
				// Nothing. People with missing titles from mods and whatnot can have holes on their map.
				Logger.Warn("Landed title " + landedTitle.first + " has no definition in counties={} section of the save game!");
			}
		}
		Logger.Info("<> " + counter + " landed titles updated.");
	}
	public void linkTitles(Titles titles) {
		int counter = 0;
		var titleData = titles.getTitles();
		foreach (var landedTitle in foundTitles)
		{
			const auto& theCapital = landedTitle.second->getCapital();
			if (!theCapital)
				continue;
			if (const auto& titleDataItr = titleData.find(theCapital->first); titleDataItr != titleData.end())
			{
				landedTitle.second->loadCapital(*titleDataItr);
				++counter;
			}
			else
			{
				Log(LogLevel::Error) << "Landed title " + landedTitle.first + " has a capital " + theCapital->first + " which has no definition!";
			}
		}
		Logger.Info("<> " + counter + " landed title capitals updated.");
	}

	
	private void registerKeys(Parser parser, ColorFactory colorFactory) 
	{
		parser.RegisterRegex(R"((e|k|d|c|b)_[A-Za-z0-9_\-\']+)", (reader, titleName) => {
			// Pull the titles beneath this one and add them to the lot, overwriting existing ones.
			var newTitle = new LandedTitles();
			newTitle.loadTitles(theStream);
			foreach (var locatedTitle: newTitle->getFoundTitles())
			foundTitles[locatedTitle.first] = locatedTitle.second;

			// And then add this one as well, overwriting existing.
			foundTitles[titleName] = newTitle;
		});
		parser.RegisterKeyword("definite_form", reader => DefiniteForm = reader.GetString() == "yes");
		parser.RegisterKeyword("landless", reader => Landless = reader.GetString() == "yes");
		parser.RegisterKeyword("color", reader => Color = colorFactory.GetColor(reader));
		parser.RegisterKeyword("capital", reader => {
			Capital = new(reader.GetString(), null);
		});
		parser.RegisterKeyword("province", reader => {
			Province = new (reader.GetInt(), null);
		});
		parser.RegisterKeyword("can_be_named_after_dynasty", reader => {
			CanBeNamedAfterDynasty = reader.GetString() == "yes";
		});
		parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}
};