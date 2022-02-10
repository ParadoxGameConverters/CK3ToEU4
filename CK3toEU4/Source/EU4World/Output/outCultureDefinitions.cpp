#include "outCultureDefinitions.h"
#include <ostream>

std::ostream& mappers::operator<<(std::ostream& output, const CultureDefinition& cultureDefinition)
{
	output << "\t\tmale_names = { ";
	for (const auto& name: cultureDefinition.maleNames)
	{
		output << "\"" << name << "\" ";
	}
	output << "}\n";
	output << "\t\t\n";
	output << "\t\tfemale_names = { ";
	for (const auto& name: cultureDefinition.femaleNames)
	{
		output << "\"" << name << "\" ";
	}
	output << "}\n";
	output << "\t\t\n";
	output << "\t\tdynasty_names = { ";
	for (const auto& name: cultureDefinition.dynastyNames)
	{
		output << "\"" << name << "\" ";
	}
	output << "}\n";
	output << "\t\t\n";
	return output;
}

std::ostream& mappers::operator<<(std::ostream& output, const CultureGroupDefinition& cultureGroupDefinition)
{
	for (const auto& [cultureName, culture]: cultureGroupDefinition.cultures)
	{
		output << "\t" << cultureName << " = {\n";
		output << *culture;
		output << "\t}\n";
	}
	output << "\t\n";
	return output;
}

std::ostream& mappers::operator<<(std::ostream& output, const CultureDefinitionsMapper& cultureDefinitionsMapper)
{
	for (const auto& [cultureGroupName, cultureGroup]: cultureDefinitionsMapper.cultureGroupsMap)
	{
		output << cultureGroupName << " = {\n";
		output << *cultureGroup;
		output << "}\n\n";
	}
	return output;
}
