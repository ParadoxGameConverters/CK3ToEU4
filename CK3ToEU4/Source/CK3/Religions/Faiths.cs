using System;
using System.Collections.Generic;
using commonItems;
using commonItems.Colors;

namespace CK3ToEU4.CK3.Religions;

class Faiths
{
	public Faiths() {}
	public Faiths(BufferedReader reader, ColorFactory colorFactory)
	{
		var parser = new Parser();
		registerKeys(parser, colorFactory);
		parser.ParseStream(reader);
	}

	public IReadOnlyDictionary<long, Faith> getFaiths() { return faiths; }

	public void linkReligions(Religions religions, Titles.Titles titles)
	{
		int counter = 0;
		var religionData = religions.getReligions();
		Dictionary<string, string> religiousHeadList; // ID, Title
		foreach (var title in titles.getTitles())
		{
			if (title.second)
			{
				religiousHeadList.emplace(std::to_string(title.second->getID()), title.first);
			}
		}
		
		foreach (var faith in faiths)
		{
			const auto& religionDataItr = religionData.find(faith.second->getReligion().first);
			if (religionDataItr != religionData.end())
			{
				faith.second->loadReligion(*religionDataItr);
				if (religiousHeadList.contains(faith.second->getReligiousHead()))
					faith.second->setReligiousHead(religiousHeadList.at(faith.second->getReligiousHead()));
				++counter;
			}
			else
			{
				throw new Exception(
					"Faith " + faith.Value.getName() + " has religion " + faith.Value.getReligion().Key + " which has no definition!");
			}
		}
		Logger.Info("<> " + counter + " faiths updated.");
	}

	private void registerKeys(Parser parser, ColorFactory colorFactory)
	{
		parser.RegisterRegex(CommonRegexes.Integer, (reader, faithId) => {
			var newFaith = new Faith(reader, long.Parse(faithId), colorFactory);
			faiths.Add(newFaith.ID, newFaith);
		});
		parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}

	private Dictionary<long, Faith> faiths;
}
