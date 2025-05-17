#include "CultureDefinitionsMapper.h"
#include "../../CK3World/Cultures/Culture.h"
#include "../../Configuration/Configuration.h"
#include "../CultureDefinitionsMapper/CultureDefiniton.h"
#include "../CultureMapper/CultureMapper.h"
#include "CommonRegexes.h"
#include "CultureGroupDefinition.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <ranges>

void mappers::CultureDefinitionsMapper::loadDefinitionsFromEU4Installation(const Configuration& theConfiguration)
{
	Log(LogLevel::Info) << "Parsing Cultures and Culture Groups";
	registerKeys();

	for (const auto& cultureFile: commonItems::GetAllFilesInFolder(theConfiguration.getEU4Path() / "common/cultures/"))
		parseFile(theConfiguration.getEU4Path() / "common/cultures" / cultureFile);

	for (const auto& cultureFile: commonItems::GetAllFilesInFolder(std::filesystem::path("blankMod/output/common/cultures/")))
		parseFile("blankMod/output/common/cultures" / cultureFile);

	clearRegisteredKeywords();
}

mappers::CultureDefinitionsMapper::CultureDefinitionsMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CultureDefinitionsMapper::registerKeys()
{
	registerRegex(R"([\w_]+)", [this](std::string cultureGroupName, std::istream& theStream) {
		auto newGroup = std::make_shared<CultureGroupDefinition>(cultureGroupName, theStream);
		if (cultureGroupsMap.contains(cultureGroupName))
		{
			for (const auto& [cultureName, culture]: newGroup->getCultures())
				cultureGroupsMap[cultureGroupName]->mergeCulture(cultureName, culture);
			cultureGroupsMap[cultureGroupName]->addMaleNames(newGroup->getMaleNames());
			cultureGroupsMap[cultureGroupName]->addFemaleNames(newGroup->getFemaleNames());
			cultureGroupsMap[cultureGroupName]->addDynastyNames(newGroup->getDynastyNames());
		}
		else
		{
			cultureGroupsMap.emplace(cultureGroupName, newGroup);
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void mappers::CultureDefinitionsMapper::buildDefinitions(const CultureMapper& cultureMapper)
{
	// We're generating new culture definitions for dynamic cultures from existing culture defs.
	// We'll be filing them into culture groups according to heritages.

	for (const auto& culture: cultureMapper.getCK3Cultures())
	{
		if (!culture->isDynamic() || culture->isEU4Ready())
			continue;

		auto newDefinition = std::make_shared<CultureDefinition>();
		newDefinition->setSourceCulture(culture);
		for (const auto& nameList: culture->getNameLists())
		{
			// these are name lists from ck3. do they match cultures? I sure hope so.
			const auto& eu4CultureNameMatch = cultureMapper.cultureNonRegionalNonReligiousMatch(nameList, "", 0, "");
			if (eu4CultureNameMatch)
			{
				// We now have an EU4 culture code we want to pick names from.
				const auto& eu4Culture = getCulture(*eu4CultureNameMatch);
				if (eu4Culture)
					assignNamesToCulture(eu4Culture, newDefinition, *eu4CultureNameMatch);
				else
					Log(LogLevel::Warning) << "Cannot find EU4 culture definitions for culture " << *eu4CultureNameMatch;
			}
			else
			{
				Log(LogLevel::Warning) << "Cannot find EU4 culture match for namelist " << nameList;
			}
		}
		assignCultureToCultureGroup(culture, newDefinition);
	}
}

void mappers::CultureDefinitionsMapper::assignCultureToCultureGroup(const std::shared_ptr<CK3::Culture>& sourceCulture,
	 const std::shared_ptr<CultureDefinition>& targetDefinition)
{
	// Locate culture grup.
	if (!sourceCulture->getHeritage().empty())
	{
		const auto& cultureGroupMatch = herritageMapper.getCultureGroupForHeritage(sourceCulture->getHeritage());
		if (cultureGroupMatch)
		{
			const auto& culgroup = getCultureGroup(*cultureGroupMatch);
			culgroup->mergeCulture(sourceCulture->getName(), targetDefinition);
		}
		else
		{
			Log(LogLevel::Warning) << "No culture group for culture " << sourceCulture->getName() << ", shoving into lost.";
			const auto& culgroup = getCultureGroup("lost_cultures_group");
			culgroup->mergeCulture(sourceCulture->getName(), targetDefinition);
		}
	}
	else
	{
		Log(LogLevel::Warning) << "No heritage for culture " << sourceCulture->getName() << "?! Shoving into lost.";
		const auto& culgroup = getCultureGroup("lost_cultures_group");
		culgroup->mergeCulture(sourceCulture->getName(), targetDefinition);
	}
}

void mappers::CultureDefinitionsMapper::assignNamesToCulture(const std::shared_ptr<CultureDefinition>& sourceCulture,
	 const std::shared_ptr<CultureDefinition>& targetDefinition,
	 const std::string& sourceCultureName) const
{
	// Are there any names?
	if (!sourceCulture->getMaleNames().empty())
	{
		// we have actual culture with names, copy the names.
		targetDefinition->addMaleNames(sourceCulture->getMaleNames());
		targetDefinition->addFemaleNames(sourceCulture->getFemaleNames());
		targetDefinition->addDynastyNames(sourceCulture->getDynastyNames());
	}
	else
	{
		// fallback - try and copy culture group names.
		auto eu4CultureGroup = getCultureGroupForCultureName(sourceCultureName);
		if (eu4CultureGroup)
		{
			// If this is empty as well, it's beyond our control.
			targetDefinition->addMaleNames(eu4CultureGroup->getMaleNames());
			targetDefinition->addFemaleNames(eu4CultureGroup->getFemaleNames());
			targetDefinition->addDynastyNames(eu4CultureGroup->getDynastyNames());
		}
	}
}

std::shared_ptr<mappers::CultureDefinition> mappers::CultureDefinitionsMapper::getCulture(const std::string& cultureName) const
{
	for (const auto& cultureGroup: cultureGroupsMap | std::views::values)
		if (cultureGroup->containsCulture(cultureName))
			return cultureGroup->getCulture(cultureName);
	return nullptr;
}

std::shared_ptr<mappers::CultureGroupDefinition> mappers::CultureDefinitionsMapper::getCultureGroup(const std::string& cultureGroupName)
{
	if (cultureGroupsMap.contains(cultureGroupName))
	{
		return cultureGroupsMap.at(cultureGroupName);
	}
	else
	{
		const auto newDefinition = std::make_shared<CultureGroupDefinition>();
		newDefinition->setName(cultureGroupName);
		cultureGroupsMap.emplace(cultureGroupName, newDefinition);
		return cultureGroupsMap.at(cultureGroupName);
	}
}

std::shared_ptr<mappers::CultureGroupDefinition> mappers::CultureDefinitionsMapper::getCultureGroupForCultureName(const std::string& cultureName) const
{
	for (const auto& cultureGroup: cultureGroupsMap | std::views::values)
		if (cultureGroup->getCultures().contains(cultureName))
			return cultureGroup;
	return nullptr;
}
