
using System;
using System.Collections.Generic;
using commonItems;
using commonItems.Colors;

namespace CK3ToEU4.CK3.CoatsOfArms;

class CoatsOfArms
{
	public CoatsOfArms()
	{
	}

	public CoatsOfArms(BufferedReader reader, ColorFactory colorFactory)
	{
		var parser = new Parser();
		registerKeys(parser, colorFactory);
		parser.ParseStream(reader);
	}
	public IReadOnlyDictionary<long, CoatOfArms> Coats => coats;
	private Dictionary<long, CoatOfArms> coats;

	public void linkParents(Titles.Titles titles)
	{
		int counter = 0;
		var titleData = titles.getTitles();
		foreach (var coat in coats)
		{
			if (coat.Value.Parent is null)
				continue;
			const auto& titleDataItr = titleData.find(coat.second->getParent()->first);
			if (titleDataItr != titleData.end())
			{
				if (!titleDataItr->second->getCoA())
					throw new Exception("CoA " + std::to_string(coat.first) + " has parent " + coat.second->getParent()->first + " which has no coat defined!");
				if (!coats.count(titleDataItr->second->getCoA()->first))
					throw new Exception(
						"CoA " + std::to_string(coat.first) + " has parent " + coat.second->getParent()->first + " which has invalid coat defined!");
				coat.second->loadParent(std::make_pair(coat.second->getParent()->first, coats[titleDataItr->second->getCoA()->first]));
				++counter;
			}
			else
			{
				throw new Exception("CoA " + std::to_string(coat.first) + " has parent " + coat.second->getParent()->first + " which is undefined!");
			}
		}
		Logger.Info("<> " + counter + " coats updated.");
	}

	:
	private void registerKeys(Parser parser, ColorFactory colorFactory)
	{
		parser.RegisterRegex(CommonRegexes.Integer, (reader, coaID) => {
			var newCoA = new CoatOfArms(reader, long.Parse(coaID), colorFactory);
			coats[newCoA.Id] = newCoA;
		});
		parser.RegisterKeyword("coat_of_arms_manager_database", reader => {
			coats = new (new CoatsOfArms(reader, colorFactory).Coats);
		});
		parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}

};