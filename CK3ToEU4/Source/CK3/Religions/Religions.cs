using System;
using System.Collections.Generic;
using commonItems;

namespace CK3ToEU4.CK3.Religions;

class Religions
{
	public Religions(BufferedReader reader)
	{
		var parser = new Parser();
		registerKeys(parser);
		parser.ParseStream(reader);
	}

	public IReadOnlyDictionary<long, Religion> getReligions() { return religions; }
	public Faith getFaiths() { return faiths; } // Use this only once in World.cpp

	void linkFaiths(Faiths theFaiths)
	{
		int counter = 0;
		var faithData = theFaiths.getFaiths();
		foreach (var religion in religions)
		{
			var religionFaiths = religion.Value.getFaiths();
			Dictionary<long, Faith> replacementMap;

			foreach (var faith in religionFaiths)
			{
				const auto& faithDataItr = faithData.find(faith.first);
				if (faithDataItr != faithData.end())
				{
					replacementMap.insert(*faithDataItr);
				}
				else
				{
					throw new Exception($"Religion {religion.Value.getName()} has faith {faith.Key} which has no definition!");
				}
			}
			religion.Value.loadFaiths(replacementMap);
			++counter;
		}
		Logger.Info("<> " + counter + " religions updated.");
	}

	private void registerKeys(Parser parser)
	{
		parser.RegisterRegex(CommonRegexes.Integer, (reader, faithID) => {
			var newReligion = new Religion(reader, long.Parse(faithID));
			religions.Add(newReligion.ID, newReligion);
		});
		parser.RegisterKeyword("religions", reader => {
			religions = new(new Religions(reader).getReligions());
		});
		parser.RegisterKeyword("faiths", reader => {
			faiths = new Faiths(reader);
		});
		parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}

	Dictionary<long, Religion> religions;
	Faiths faiths;
};