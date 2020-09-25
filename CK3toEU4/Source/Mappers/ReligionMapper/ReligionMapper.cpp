#include "ReligionMapper.h"
#include "../../CK3World/Religions/Faith.h"
#include "../../CK3World/Religions/Faiths.h"
#include "ReligionDefinitionMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "ReligionMapping.h"

mappers::ReligionMapper::ReligionMapper()
{
	LOG(LogLevel::Info) << "-> Parsing religion mappings.";
	registerKeys();
	parseFile("configurables/religion_map.txt");
	clearRegisteredKeywords();
	LOG(LogLevel::Info) << "<> Loaded " << CK3toEU4ReligionMap.size() << " religious links.";
}

mappers::ReligionMapper::ReligionMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionMapper::registerKeys()
{
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream) {
		const ReligionMapping theMapping(theStream);
		for (const auto& ck3Religion: theMapping.getCK3Religions())
		{
			CK3toEU4ReligionMap.insert(std::make_pair(ck3Religion, theMapping.getEU4Religion()));
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::ReligionMapper::getEU4ReligionForCK3Religion(const std::string& ck3Religion) const
{
	const auto& mapping = CK3toEU4ReligionMap.find(ck3Religion);
	if (mapping != CK3toEU4ReligionMap.end())
		return mapping->second;
	return std::nullopt;
}

void mappers::ReligionMapper::importCK3Faiths(const CK3::Faiths& faiths, ReligionDefinitionMapper& definitions)
{
	for (const auto& faith: faiths.getFaiths())
	{
		if (!getEU4ReligionForCK3Religion(faith.second->getName()))
		{
			// This is a new faith.
			importCK3Faith(*faith.second, definitions);
		}
	}
}

void mappers::ReligionMapper::importCK3Faith(const CK3::Faith& faith, ReligionDefinitionMapper& definitions)
{
	// Hello, imported CK3 dynamic faith.
	const auto& origName = faith.getName();
	const auto faithName = "converted_" + origName; // makes them easier to notice
	const auto& displayName = faith.getCustomAdj(); // Catholic, not catholicism
	LocBlock locBlock;
	locBlock.english = displayName;
	locBlock.french = displayName;
	locBlock.german = displayName;
	locBlock.spanish = displayName; // Ck3 save only stores the one display name, so we have no choice but to copy it around.
	localizations.insert(std::pair(faithName, locBlock));

	// Grab the source definitions from the originating faith (unreformed one)
	std::string country;
	std::string countrySecondary;
	std::string province;
	std::string unique;
	std::string nonUnique;
	if (!faith.getTemplate().empty()) // for a NakedMan custom religion, this would probably be ck3's "adamites".
	{
		const auto& dstReligion = getEU4ReligionForCK3Religion(faith.getTemplate()); // and this would map into eu4's "adamites"
		if (dstReligion)
		{
			const auto& match = definitions.getDefinition(*dstReligion);
			if (match)
			{
				country += match->getCountry();
				countrySecondary += match->getCountrySecondary();
				province += match->getProvince();
				unique = match->getUnique(); // overriding uniques, always.
				nonUnique += match->getNonUnique();
			}
			else
			{
				Log(LogLevel::Warning) << "EU4 Religion Template " << *dstReligion << " does not exist! Check religion_globals.txt!";
			}
		}
		else
		{
			Log(LogLevel::Warning) << "CK3-EU4 Religion Mapping for " << faith.getTemplate() << " does not exist! Check religion_map.txt! We will be scraping defaults.";			
		}
	}
	else
	{
		Log(LogLevel::Warning) << "CK3 Religion Template for " << origName << " does not exist! We will be scraping defaults.";
	}

	for (const auto& doctrine: faith.getDoctrines())
	{
		const auto& match = definitions.getDefinition(doctrine);
		if (match)
		{
			country += match->getCountry();
			countrySecondary += match->getCountrySecondary();
			province += match->getProvince();
			unique = match->getUnique(); // overriding uniques, always.
			nonUnique += match->getNonUnique();			
		}
		else
		{
			Log(LogLevel::Warning) << "Doctrine  definition for " << doctrine << " does not exist! Check doctrine_definitions.txt!";
		}
	}

	// Store new religion for later processing and output.
	generatedReligion newReligion;
	newReligion.country = country;
	newReligion.countrySecondary = countrySecondary;
	newReligion.province = province;
	newReligion.unique = unique;
	newReligion.nonUnique = nonUnique;
	newReligion.icon = definitions.getNextIcon();
	newReligion.iconPath = faith.getIconPath();
	if (newReligion.iconPath.empty())
	{
		Log(LogLevel::Warning) << "CK3 Icon for " << origName << " does not exist! Religion will have no icon!";
	}
	newReligion.color = faith.getColor();
	if (!newReligion.color)
	{
		Log(LogLevel::Warning) << "CK3 Color for " << origName << " does not exist! Religion will be black!";
	}	
	generatedReligions.emplace_back(newReligion);

	// and register it.
	CK3toEU4ReligionMap.insert(std::pair(origName, faithName));
}
