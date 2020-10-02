#include "ReligionMapper.h"
#include "../../CK3World/Religions/Faith.h"
#include "../../CK3World/Religions/Faiths.h"
#include "../ReligionDefinitionMapper/ReligionDefinitionMapper.h"
#include "../ReligionGroupScraper/ReligionGroupScraper.h"
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

void mappers::ReligionMapper::importCK3Faiths(const CK3::Faiths& faiths,
	 ReligionDefinitionMapper& religionDefinitionMapper,
	 const ReligionGroupScraper& religionGroupScraper,
	 const LocalizationMapper& localizationMapper)
{
	for (const auto& faith: faiths.getFaiths())
	{
		if (!getEU4ReligionForCK3Religion(faith.second->getName()))
		{
			// This is a new faith.
			importCK3Faith(*faith.second, religionDefinitionMapper, religionGroupScraper, localizationMapper);
		}
	}
}

void mappers::ReligionMapper::importCK3Faith(const CK3::Faith& faith,
	 ReligionDefinitionMapper& religionDefinitionMapper,
	 const ReligionGroupScraper& religionGroupScraper,
	 const LocalizationMapper& localizationMapper)
{
	// Hello, imported CK3 dynamic faith.
	const auto& origName = faith.getName();
	const auto faithName = "converted_" + origName; // makes them easier to notice
	const auto& displayName = faith.getCustomAdj(); // Catholic, not catholicism
	const auto description = faith.getDescription();
	LocBlock locBlock;
	locBlock.english = displayName;
	locBlock.french = displayName;
	locBlock.german = displayName;
	locBlock.spanish = displayName; // Ck3 save only stores the one display name, so we have no choice but to copy it around.
	localizations.insert(std::pair(faithName, locBlock));
	LocBlock descBlock;
	if (!description.empty())
	{
		descBlock.english = description;
		descBlock.french = description;
		descBlock.german = description;
		descBlock.spanish = description;
	}
	else
	{
		const auto& match = localizationMapper.getLocBlockForKey(faith.getTemplate()); // We'll attempt to copy over parent desc.
		if (match)
		{
			descBlock.english = match->english;
			descBlock.french = match->french;
			descBlock.german = match->german;
			descBlock.spanish = match->spanish;
		}
	}
	// rather have it empty than display raw key ingame.
	localizations.insert(std::pair(faithName + "_religion_desc", descBlock));

	// Rebels
	LocBlock rebelBlock;
	//Title
	rebelBlock.english = "Religious";
	rebelBlock.french = "Dévots";
	rebelBlock.german = "Religiöse";
	rebelBlock.spanish = "Religioso";
	localizations.insert(std::pair(faithName + "_rebels_title", rebelBlock));
	//Name
	rebelBlock.english = "$RELIGION$ Zealots";
	rebelBlock.french = "Fanatiques $RELIGION$";
	rebelBlock.german = "$RELIGION$-Fanatiker";
	rebelBlock.spanish = "Fanáticos de $RELIGION$";
	localizations.insert(std::pair(faithName + "_rebels_name", rebelBlock));
	//Desc
	rebelBlock.english = "Religious fanatics tend to rise up in provinces controlled by nations of a different faith. They seek to spread their faith and smite all unbelievers.";
	rebelBlock.french = "Ces fanatiques religieux ont tendance à se soulever dans les provinces contrôlées par des nations de confession différente. Ils cherchent à répandre leur foi et s\'en prennent à tous les infidèles.";
	rebelBlock.german = "Religiöse Fanatiker neigen dazu, sich in Provinzen zu erheben, die von Nationen eines anderen Glaubens kontrolliert werden. Sie versuchen, ihren Glauben zu verbreiten und alle Ungläubigen zu quälen.";
	rebelBlock.spanish = "Los fanáticos religiosos suelen aparecer en provincias controladas por naciones que practican una creencia diferente. Buscan propagar su fe y sofocar a todos los infieles.";
	localizations.insert(std::pair(faithName + "_rebels_desc", rebelBlock));
	//Army
	rebelBlock.english = "$RELIGION$ Army";
	rebelBlock.french = "Armée $RELIGION$";
	rebelBlock.german = "$RELIGION$-Armee";
	rebelBlock.spanish = "Ejército $RELIGION$";
	localizations.insert(std::pair(faithName + "_rebels_army", rebelBlock));

	// Grab the source definitions from the originating faith (unreformed one)
	std::string allowedConversion;
	std::string country;
	std::string countrySecondary;
	std::string province;
	std::string unique;
	std::string nonUnique;
	std::string eu4ParentReligion;
	std::string staticBlob;
	if (!faith.getTemplate().empty()) // for a NakedMan custom religion, this would probably be ck3's "adamites".
	{
		const auto& dstReligion = getEU4ReligionForCK3Religion(faith.getTemplate()); // and this would map into eu4's "adamites"
		if (dstReligion)
		{
			eu4ParentReligion = *dstReligion;
			const auto& match = religionDefinitionMapper.getStaticBlob(*dstReligion);
			if (match)
			{
				staticBlob = *match;
			}
			else
			{
				Log(LogLevel::Warning) << "EU4 Religion Template " << *dstReligion << " does not exist! Check religion_globals.txt!";
			}
		}
		else
		{
			Log(LogLevel::Warning) << "CK3-EU4 Religion Mapping for " << faith.getTemplate()
										  << " does not exist! Check religion_map.txt! We will be scraping defaults.";
		}
	}
	else
	{
		Log(LogLevel::Warning) << "CK3 Religion Template for " << origName << " does not exist! We will be scraping defaults.";
	}

	for (const auto& doctrine: faith.getDoctrines())
	{
		const auto& match = religionDefinitionMapper.getDefinition(doctrine);
		if (match)
		{
			if (!match->getAllowedConversion().empty())
				allowedConversion += match->getAllowedConversion() + "\n";
			if (!match->getCountry().empty())
				country += match->getCountry() + "\n";
			if (!match->getCountrySecondary().empty())
				countrySecondary += match->getCountrySecondary() + "\n";
			if (!match->getProvince().empty())
				province += match->getProvince() + "\n";
			if (!match->getUnique().empty())
				unique = match->getUnique(); // overriding uniques, always.
			if (!match->getNonUnique().empty())
				nonUnique += match->getNonUnique() + "\n";
		}
		else
		{
			Log(LogLevel::Warning) << "Doctrine  definition for " << doctrine << " does not exist! Check doctrine_definitions.txt!";
		}
	}

	// Store new religion for later processing and output.
	EU4::GeneratedReligion newReligion;
	newReligion.allowedConversion = allowedConversion;
	newReligion.country = country;
	newReligion.countrySecondary = countrySecondary;
	newReligion.province = province;
	newReligion.unique = unique;
	newReligion.nonUnique = nonUnique;
	newReligion.icon = religionDefinitionMapper.getNextIcon();
	newReligion.staticBlob = staticBlob;
	newReligion.iconPath = faith.getIconPath();
	newReligion.parent = eu4ParentReligion;
	if (newReligion.iconPath.empty())
	{
		Log(LogLevel::Warning) << "CK3 Icon for " << origName << " does not exist! Religion will have no icon!";
	}
	newReligion.color = faith.getColor();
	if (!newReligion.color)
	{
		Log(LogLevel::Warning) << "CK3 Color for " << origName << " does not exist! Religion will be black!";
	}
	const auto& match = religionGroupScraper.getReligionGroupForReligion(eu4ParentReligion);
	if (match)
	{
		newReligion.religionGroup = *match;
	}
	else
	{
		Log(LogLevel::Warning) << "EU4 religion group for " << eu4ParentReligion
									  << " could not be found! Check religion defines in blankmod/output/common/religions! Will appear standalone!";
	}
	newReligion.name = faithName;

	generatedReligions.emplace_back(newReligion);

	// and register it.
	CK3toEU4ReligionMap.insert(std::pair(origName, faithName));
}
