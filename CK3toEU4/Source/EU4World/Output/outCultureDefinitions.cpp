#include "outCultureDefinitions.h"
#include "../../CK3World/Cultures/Culture.h"
#include <ostream>

std::ostream& mappers::operator<<(std::ostream& output, const CultureDefinition& cultureDefinition)
{
	if (!cultureDefinition.maleNames.empty())
	{
		output << "\t\tmale_names = { ";
		for (const auto& name: cultureDefinition.maleNames)
		{
			output << "\"" << name << "\" ";
		}
		output << "}\n";
	}
	if (!cultureDefinition.femaleNames.empty())
	{
		output << "\t\tfemale_names = { ";
		for (const auto& name: cultureDefinition.femaleNames)
		{
			output << "\"" << name << "\" ";
		}
		output << "}\n";
	}
	if (!cultureDefinition.dynastyNames.empty())
	{
		output << "\t\tdynasty_names = { ";
		for (const auto& name: cultureDefinition.dynastyNames)
		{
			output << "\"" << name << "\" ";
		}
		output << "}\n";
	}
	return output;
}

std::ostream& mappers::operator<<(std::ostream& output, const CultureGroupDefinition& cultureGroupDefinition)
{
	// We only dump dynamic cultures.

	for (const auto& [cultureName, culture]: cultureGroupDefinition.cultures)
	{
		if (culture->getSourceCulture() && culture->getSourceCulture()->isDynamic())
		{
			output << "\t" << cultureName << " = {\n";
			output << *culture;
			output << "\t}\n";
		}
	}
	return output;
}

std::ostream& mappers::operator<<(std::ostream& output, const CultureDefinitionsMapper& cultureDefinitionsMapper)
{
	// We only dump culture groups containing dynamic cultures.

	for (const auto& [cultureGroupName, cultureGroup]: cultureDefinitionsMapper.cultureGroupsMap)
	{
		bool interesting = false;
		for (const auto& culture: cultureGroup->getCultures())
		{
			if (culture.second->getSourceCulture() && culture.second->getSourceCulture()->isDynamic())
			{
				interesting = true;
				break;
			}
		}

		if (interesting)
		{
			output << cultureGroupName << " = {\n";
			output << *cultureGroup;
			output << "}\n\n";
		}
	}
	return output;
}
