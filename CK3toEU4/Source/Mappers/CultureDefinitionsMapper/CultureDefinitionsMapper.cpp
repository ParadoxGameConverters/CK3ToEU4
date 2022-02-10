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

void mappers::CultureDefinitionsMapper::initForEU4(const Configuration& theConfiguration)
{
	Log(LogLevel::Info) << "Parsing Cultures and Culture Groups";
	registerKeys();

	for (const auto& cultureFile: commonItems::GetAllFilesInFolder(theConfiguration.getEU4Path() + "/common/cultures/"))
		parseFile(theConfiguration.getEU4Path() + "/common/cultures/" + cultureFile);

	for (const auto& cultureFile: commonItems::GetAllFilesInFolder("blankMod/output/common/cultures/"))
		parseFile("blankMod/output/common/cultures/" + cultureFile);

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
		}
		else
		{
			cultureGroupsMap.insert(std::make_pair(cultureGroupName, newGroup));
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::shared_ptr<mappers::CultureGroupDefinition> mappers::CultureDefinitionsMapper::getGroupForCulture(const std::string& cultureName) const
{
	for (const auto& cultureGroupItr: cultureGroupsMap)
		if (cultureGroupItr.second->containsCulture(cultureName))
			return cultureGroupItr.second;
	return nullptr;
}

void mappers::CultureDefinitionsMapper::buildDefinitions(const CultureMapper& cultureMapper)
{
	// We're generating new culture definitions for dynamic cultures from existing culture defs.
	// We'll be filing them into culture groups according to heritages.

	for (const auto& culture: cultureMapper.getCK3Cultures() | std::views::values)
	{
		if (culture->isDynamic() && !culture->isEU4Ready())
		{
			auto newDefinition = std::make_shared<CultureDefinition>();
			for (const auto& nameList: culture->getNameLists())
			{
				// these are name lists from ck3. do they match cultures? I sure hope so.
				const auto& match = cultureMapper.cultureNonRegionalNonReligiousMatch(nameList, "", 0, "");
				if (match)
				{
					// We now have an EU4 culture code we want to pick names from.
					const auto& eu4Culture = getCulture(*match);
					if (eu4Culture)
					{
						// we have actual culture, copy the names.
						newDefinition->addMaleNames(eu4Culture->getMaleNames());
						newDefinition->addFemaleNames(eu4Culture->getFemaleNames());
						newDefinition->addDynastyNames(eu4Culture->getDynastyNames());
						// Locate culture grup.
						const auto& culgroup = getCultureGroup("new");
						culgroup->mergeCulture(culture->getName(), newDefinition);
					}
					else
					{
						Log(LogLevel::Warning) << "Cannot find EU4 culture definitions for culture " << *match;
					}
				}
				else
				{
					Log(LogLevel::Warning) << "Cannot find EU4 culture match for namelist " << nameList;
				}
			}
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
