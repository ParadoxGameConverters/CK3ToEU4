using System.Collections.Generic;
using commonItems;

namespace CK3ToEU4.CK3.Cultures;

class Culture
{
	public Culture(BufferedReader reader, long theID)
	{
		ID = theID;
		
		var parser = new Parser();
		registerKeys(parser);
		parser.ParseStream(reader);
	}

	public long getID() { return ID; }
	public bool isEU4Ready() { return eu4Ready; }
	public bool isDynamic() { return dynamic; }
	public  const auto& getLocalizedName() { return localizedName; }
	public  const auto& getName() { return name; }
	public  const auto& getNameLists() { return nameLists; }
	public  const auto& getHeritage() { return heritage; }
	public  const auto& getTemplate() { return culture_template; }
	public string getEthos() { return ethos; }
	public  const auto& getTraditions() { return traditions; }

	public void setDynamic() { dynamic = true; }
	public void concoctCultureName(LocalizationMapper localizationMapper,
		CultureMapper cultureMapper,
		Dictionary<string, int> cultureNamingCounter)
	{
		/* This function is responsible for determining what a culture is and where it's going. Base/vanilla cultures are known to us but
		 * hybrids and divergences most certainly are not. We can try to normalize some of them like Swiss (hybrid) or Austrian (divergence) into
		 * eu4 cultures (sidestepping cultural mapping altogether), and if that fails we can generate dynamic cultures and file them in culture
		 * groups according to their heritages. In this function we do the first half.
		 */

		// Is this a base ck3 culture?
		if (culture_template)
		{
			name = *culture_template;
			return;
		}

		// Does this culture have a name? If not that means the player was very funny. We'll do the same.
		if (!localizedName)
		{
			name = "noname";
			return;
		}

		/* We have a divergent culture. Hybrids and divergents are by definition eu4-ready cultures but:
		 * 1. we allow for overrides using "ck3 = culture" mappings
		 * 2. not all of them have eu4 definitions which we'll have to generate.
		 * If a culture is not in "eu4 = " target block then 2) applies and we need to know this.
		 */

		// Can we reverse map it via localization into some common base like "austrian"?
		var match = localizationMapper.reverseLookupCultureName(*localizedName);
		if (match)
		{
			var strippedName = match;
			strippedName = strippedName.substr(0, strippedName.size() - 5);
			if (cultureMapper.getTargetCultures().contains(strippedName))
			{
				// this is a full-flegded eu4 culture with predefined definitions.
				name = strippedName;
				eu4Ready = true;
				return;
			}

			if (cultureMapper.getSourceCultures().contains(strippedName))
			{
				// this is a culture we've mapped to something else. Proceed normally as if it were vanilla ck3 culture.
				name = strippedName;
				return;
			}
		}

		// Now everything else, we need to Concoct the culture name, finally.
		name = "dynamic-";
		foreach (var entry in nameLists)
		{
			// Enery name component must be mapped to some base eu4 culture, so that eu4tovic2 can decompose it.
			var cultureMatch = cultureMapper.cultureNonRegionalNonReligiousMatch(entry, "", 0, "");
			if (cultureMatch)
			{
				name += *cultureMatch + "-";
			}
			else
			{
				Log(LogLevel::Warning) << "Mapping " << entry << " to an EU4 culture failed! Check mappings!";
				name += entry + "-";
			}
		}
		name += "culture";

		// did we see this culture before, elsewhere?

		if (cultureNamingCounter.contains(name))
		{

			++cultureNamingCounter.at(name);
			name += "-num" + std::to_string(cultureNamingCounter.at(name));
		}
		else
		{
			cultureNamingCounter.emplace(name, 1);
			name += "-num1";
		}

		dynamic = true;
	}


  
	private void registerKeys(Parser parser)
	{
		parser.RegisterKeyword("culture_template", reader => {
			culture_template = reader.GetString();
		});
		parser.RegisterKeyword("name", reader => {
			localizedName = reader.GetString();
		});
		parser.RegisterKeyword("heritage", reader => {
			heritage = reader.GetString();
		});
		parser.RegisterKeyword("ethos", reader => {
			ethos = reader.GetString();
		});
		parser.RegisterKeyword("traditions", reader => {
			traditions = reader.GetStrings();
		});
		parser.RegisterKeyword("name_list", reader => {
			var temp = reader.GetString();
			if (temp.Length > 10)
			{
				temp = temp.substr(10, temp.size()); // drop "name_list_", leave "polish"
				nameLists.insert(temp);
			}
		});
		parser.RegisterRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}

	private long ID = 0;
	private bool eu4Ready = false; // this culture has eu4 match and needs zero processing
	private bool dynamic = false;  // this culture is dynamic and will need generation of cultural data

	private string? culture_template; // this has data only for base ck3 cultures, like czech or german
	private string? localizedName;		// this can be anything - user input or localized name in a particular language game is running.
	private string heritage;								// all cultures should have this.
	private HashSet<string> nameLists;					// We use these to generate dynamic culture code names, in lack of a better solution.
	private string ethos;									// used to generate custom ideas for custom tags with a custom culture
	private std::vector<string> traditions;			// used to generate custom ideas for custom tags with a custom culture

	private string name; // calculated value from all of the above - can be either *eu4* culture, ck3 vanilla, or anything in between.
};