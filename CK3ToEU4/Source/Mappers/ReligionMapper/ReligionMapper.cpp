#include "ReligionMapper.h"
#include "../../CK3World/Religions/Faith.h"
#include "../../CK3World/Religions/Faiths.h"
#include "../ReligionDefinitionMapper/ReligionDefinitionMapper.h"
#include "../ReligionGroupScraper/ReligionGroupScraper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "ReligionMapping.h"

mappers::ReligionMapper::ReligionMapper()
{
	Log(LogLevel::Info) << "-> Parsing religion mappings.";
	registerKeys();
	parseFile("configurables/religion_map.txt");
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << CK3ToEU4ReligionMap.size() << " religious links.";
}

mappers::ReligionMapper::ReligionMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		const ReligionMapping theMapping(theStream);
		auto& tempHead = theMapping.getReligiousHead();
		if (tempHead != std::nullopt)
		{
			eu4ReligionStruct tempStruct;
			tempStruct.eu4religion = theMapping.getEU4Religion();
			tempStruct.eu4school = theMapping.getEU4School();
			tempStruct.religiousHead = *tempHead;
			ReligionHeadToEU4ReligionMap.emplace(*tempHead, tempStruct);
		}
		for (const auto& ck3Religion: theMapping.getCK3Religions())
		{
			eu4ReligionStruct tempStruct;
			tempStruct.eu4religion = theMapping.getEU4Religion();
			tempStruct.eu4school = theMapping.getEU4School();
			tempStruct.religiousHead = tempHead;
			CK3ToEU4ReligionMap.emplace(ck3Religion, tempStruct);
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::ReligionMapper::getEU4ReligionForCK3Religion(const std::string& ck3Religion, const std::string& ck3ReligiousHead) const
{
	if (const auto& mapping = ReligionHeadToEU4ReligionMap.find(ck3ReligiousHead); mapping != ReligionHeadToEU4ReligionMap.end())
		return ReligionHeadToEU4ReligionMap.find(ck3ReligiousHead)->second.eu4religion;
	if (const auto& mapping = CK3ToEU4ReligionMap.find(ck3Religion); mapping != CK3ToEU4ReligionMap.end())
		return mapping->second.eu4religion;
	return std::nullopt;
}

std::optional<std::string> mappers::ReligionMapper::getEU4SchoolForCK3Religion(const std::string& ck3Religion) const
{
	if (const auto& mapping = CK3ToEU4ReligionMap.find(ck3Religion); mapping != CK3ToEU4ReligionMap.end())
		return mapping->second.eu4school;
	return std::nullopt;
}

void mappers::ReligionMapper::importCK3Faiths(const CK3::Faiths& faiths,
	 ReligionDefinitionMapper& religionDefinitionMapper,
	 const ReligionGroupScraper& religionGroupScraper,
	 const LocalizationMapper& localizationMapper)
{
	bool hasReformedAfrica = false;
	for (const auto& faith: faiths.getFaiths())
	{
		if (!getEU4ReligionForCK3Religion(faith.second->getName(), faith.second->getReligiousHead()))
		{
			// This is a new faith.
			importCK3Faith(*faith.second, religionDefinitionMapper, religionGroupScraper, localizationMapper);
		}
		else if (getEU4ReligionForCK3Religion(faith.second->getName(), faith.second->getReligiousHead()) == "west_african_pagan" &&
					!hasReformedAfrica) // Because there are several West African Religions
		{
			reformedReligions.emplace_back("west_african_pagan");
			hasReformedAfrica = true;
		}
		else if (faith.second->getReformedFlag()) // This is for unreformed religions that have been reformed (Game only stores this info in the old religon)
		{
			const auto& displayName = faith.second->getCustomAdj(); // Catholic, not catholicism
			LocBlock locBlock;
			locBlock.english = displayName; // CK3 already calls it "Old Religion", this will just overwrite the default EU4 localization calling it something
													  // else.
			locBlock.french = displayName;
			locBlock.german = displayName;
			locBlock.spanish = displayName;
			localizations.insert(std::pair(faith.second->getName(), locBlock));
			CK3Doctrines.emplace(faith.second->getTemplate(), faith.second->getDoctrines());

			reformedReligions.emplace_back(faith.second->getTemplate());
		}
	}
}

void mappers::ReligionMapper::importCK3Faith(const CK3::Faith& faith,
	 ReligionDefinitionMapper& religionDefinitionMapper,
	 const ReligionGroupScraper& religionGroupScraper,
	 const LocalizationMapper& localizationMapper)
{
	// Hello, imported CK3 dynamic faith.
	auto origName = faith.getName();
	auto faithName = "converted_" + origName;			// makes them easier to notice
	const auto& displayName = faith.getCustomAdj(); // Catholic, not catholicism
	const auto& description = faith.getDescription();
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
	// Doctrines Stored
	CK3Doctrines.emplace(faithName, faith.getDoctrines());

	// Rebels
	LocBlock rebelBlock;
	// Title
	rebelBlock.english = "Religious";
	rebelBlock.french = "D�vots";
	rebelBlock.german = "Religi�se";
	rebelBlock.spanish = "Religioso";
	localizations.insert(std::pair(faithName + "_rebels_title", rebelBlock));
	// Name
	rebelBlock.english = "$RELIGION$ Zealots";
	rebelBlock.french = "Fanatiques $RELIGION$";
	rebelBlock.german = "$RELIGION$-Fanatiker";
	rebelBlock.spanish = "Fan�ticos de $RELIGION$";
	localizations.insert(std::pair(faithName + "_rebels_name", rebelBlock));
	// Desc
	rebelBlock.english =
		 "Religious fanatics tend to rise up in provinces controlled by nations of a different faith. They seek to spread their faith and smite all unbelievers.";
	rebelBlock.french =
		 "Ces fanatiques religieux ont tendance � se soulever dans les provinces contr�l�es par des nations de confession diff�rente. Ils cherchent � r�pandre "
		 "leur foi et s\'en prennent � tous les infid�les.";
	rebelBlock.german =
		 "Religi�se Fanatiker neigen dazu, sich in Provinzen zu erheben, die von Nationen eines anderen Glaubens kontrolliert werden. Sie versuchen, ihren "
		 "Glauben zu verbreiten und alle Ungl�ubigen zu qu�len.";
	rebelBlock.spanish =
		 "Los fan�ticos religiosos suelen aparecer en provincias controladas por naciones que practican una creencia diferente. Buscan propagar su fe y sofocar "
		 "a todos los infieles.";
	localizations.insert(std::pair(faithName + "_rebels_desc", rebelBlock));
	// Army
	rebelBlock.english = "$RELIGION$ Army";
	rebelBlock.french = "Arm�e $RELIGION$";
	rebelBlock.german = "$RELIGION$-Armee";
	rebelBlock.spanish = "Ej�rcito $RELIGION$";
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
		const auto& dstReligion = getEU4ReligionForCK3Religion(faith.getTemplate(), faith.getReligiousHead()); // and this would map into eu4's "adamites"
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
		// Try with a self-similar religion. *Any* self-similar religion. At this point we don't care.
		for (const auto& similarFaith: ck3ReligionScraper.getSimilarFaiths(origName))
		{
			if (const auto& dstReligion = getEU4ReligionForCK3Religion(similarFaith, faith.getReligiousHead()); dstReligion)
			{
				eu4ParentReligion = *dstReligion;
				Log(LogLevel::Warning) << origName << " is unknown to the converter. We found a \"suitable\" replacement with: " << similarFaith << "->"
											  << eu4ParentReligion << ". Overriding.";
				if (const auto& match = religionDefinitionMapper.getStaticBlob(*dstReligion); match)
					staticBlob = *match;
				break;
			}
		}
		if (eu4ParentReligion.empty())
			Log(LogLevel::Warning) << "CK3 Religion Template for " << origName << " does not exist! We will be scraping defaults.";
	}

	short secondaryCount = 0;
	short countryCount = 0;
	for (const auto& doctrine: faith.getDoctrines())
	{
		const auto& match = religionDefinitionMapper.getDefinition(doctrine);
		if (match)
		{
			if (!match->getAllowedConversion().empty())
				allowedConversion += match->getAllowedConversion() + "\n";
			if (!match->getCountry().empty() && countryCount < 4) // This should prevent religions from having too many modifiers at once
			{
				country += match->getCountry() + "\n";
				countryCount++;
			}
			if (!match->getCountrySecondary().empty() && secondaryCount < 2) // Secondary is limited to two
			{
				countrySecondary += match->getCountrySecondary() + "\n";
				secondaryCount++;
			}
			if (!match->getProvince().empty())
				province += match->getProvince() + "\n";
			if (!match->getUnique().empty())
			{
				if (match->getUnique().find("personal_deity = yes") != std::string::npos)
				{
					if (eu4ParentReligion.find("germanic_pagan") != std::string::npos || eu4ParentReligion.find("norse_pagan") != std::string::npos)
					{
						deityCount++;
						createPersonalDeities(eu4ParentReligion, deityCount);
						unique = match->getUnique(); // overriding uniques, always.
					}
					// Do nothing, not even override, if the faith is based on neither of the above faiths
				}
				else
					unique = match->getUnique(); // overriding uniques, always.
			}
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
	if (unique.find("personal_deity = yes") != std::string::npos)
		newReligion.personalDeityNumber = deityCount;
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
	eu4ReligionStruct CK3Faith;
	CK3Faith.eu4religion = faithName;
	CK3Faith.religiousHead = faith.getReligiousHead();
	CK3ToEU4ReligionMap.emplace(origName, CK3Faith);
}

void mappers::ReligionMapper::createPersonalDeities(const std::string& parentReligion, const short deityCount)
{
	// Personal Deities
	LocBlock deityBlock;
	if (parentReligion.find("germanic_pagan") != std::string::npos)
	{
		// Names
		deityBlock.english = "Tiwaz";
		deityBlock.french = "Tiwaz";
		deityBlock.german = "Tiwaz";
		deityBlock.spanish = "Tiwaz";
		localizations.insert(std::pair("tiwaz_" + std::to_string(deityCount), deityBlock));
		deityBlock.english = "Wodan";
		deityBlock.french = "Wodan";
		deityBlock.german = "Wodan";
		deityBlock.spanish = "Wodan";
		localizations.insert(std::pair("wodan_" + std::to_string(deityCount), deityBlock));
		deityBlock.english = "Dunar";
		deityBlock.french = "Dunar";
		deityBlock.german = "Dunar";
		deityBlock.spanish = "Dunar";
		localizations.insert(std::pair("dunar_" + std::to_string(deityCount), deityBlock));
		deityBlock.english = "Frijjo";
		deityBlock.french = "Frijjo";
		deityBlock.german = "Frijjo";
		deityBlock.spanish = "Frijjo";
		localizations.insert(std::pair("frijjo_" + std::to_string(deityCount), deityBlock));
		deityBlock.english = "Austro";
		deityBlock.french = "Austro";
		deityBlock.german = "Austro";
		deityBlock.spanish = "Austro";
		localizations.insert(std::pair("austro_" + std::to_string(deityCount), deityBlock));
		deityBlock.english = "Fraujaz";
		deityBlock.french = "Fraujaz";
		deityBlock.german = "Fraujaz";
		deityBlock.spanish = "Fraujaz";
		localizations.insert(std::pair("fraujaz_" + std::to_string(deityCount), deityBlock));
	}
	else if (parentReligion.find("norse_pagan") != std::string::npos)
	{
		// Names
		deityBlock.english = "Odin";
		deityBlock.french = "Odin";
		deityBlock.german = "Odin";
		deityBlock.spanish = "Odin";
		localizations.insert(std::pair("odin_" + std::to_string(deityCount), deityBlock));
		deityBlock.english = "Freya";
		deityBlock.french = "Freya";
		deityBlock.german = "Freya";
		deityBlock.spanish = "Freya";
		localizations.insert(std::pair("freya_" + std::to_string(deityCount), deityBlock));
		deityBlock.english = "Tor";
		deityBlock.french = "Tor";
		deityBlock.german = "Tor";
		deityBlock.spanish = "Tor";
		localizations.insert(std::pair("tor_" + std::to_string(deityCount), deityBlock));
		deityBlock.english = "Tyr";
		deityBlock.french = "Tyr";
		deityBlock.german = "Tyr";
		deityBlock.spanish = "Tyr";
		localizations.insert(std::pair("tyr_" + std::to_string(deityCount), deityBlock));
		deityBlock.english = "Njord";
		deityBlock.french = "Njord";
		deityBlock.german = "Njord";
		deityBlock.spanish = "Njord";
		localizations.insert(std::pair("njord_" + std::to_string(deityCount), deityBlock));
		deityBlock.english = "Snotra";
		deityBlock.french = "Snotra";
		deityBlock.german = "Snotra";
		deityBlock.spanish = "Snotra";
		localizations.insert(std::pair("snotra_" + std::to_string(deityCount), deityBlock));
	}
}