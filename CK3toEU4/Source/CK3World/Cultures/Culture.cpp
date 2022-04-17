#include "Culture.h"
#include "../../Mappers/CultureMapper/CultureMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Culture::Culture(std::istream& theStream, long long theID): ID(theID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Culture::registerKeys()
{
	registerKeyword("culture_template", [this](std::istream& theStream) {
		culture_template = commonItems::getString(theStream);
	});
	registerKeyword("name", [this](std::istream& theStream) {
		localizedName = commonItems::getString(theStream);
	});
	registerKeyword("heritage", [this](std::istream& theStream) {
		heritage = commonItems::getString(theStream);
	});
	registerKeyword("ethos", [this](std::istream& theStream) {
		ethos = commonItems::singleString(theStream).getString();
	});
	registerKeyword("traditions", [this](std::istream& theStream) {
		traditions = commonItems::getStrings(theStream);
	});
	registerKeyword("name_list", [this](std::istream& theStream) {
		auto temp = commonItems::getString(theStream);
		if (temp.size() > 10)
		{
			temp = temp.substr(10, temp.size()); // drop "name_list_", leave "polish"
			nameLists.insert(temp);
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void CK3::Culture::concoctCultureName(const mappers::LocalizationMapper& localizationMapper,
	 const mappers::CultureMapper& cultureMapper,
	 std::map<std::string, int>& cultureNamingCounter)
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
	const auto& match = localizationMapper.reverseLookupCultureName(*localizedName);
	if (match)
	{
		auto strippedName = *match;
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
	for (const auto& entry: nameLists)
	{
		// Enery name component must be mapped to some base eu4 culture, so that eu4tovic2 can decompose it.
		const auto& cultureMatch = cultureMapper.cultureNonRegionalNonReligiousMatch(entry, "", 0, "");
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
