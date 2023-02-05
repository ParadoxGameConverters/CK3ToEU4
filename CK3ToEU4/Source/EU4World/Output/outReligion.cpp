#include "outReligion.h"

std::ostream& EU4::operator<<(std::ostream& output, const GeneratedReligion& religion)
{
	if (!religion.religionGroup.empty())
	{
		output << religion.religionGroup << " = {\n";
	}
	else
	{
		output << religion.name << " = {\n";
	}
	output << "\t" << religion.name << " = {\n";
	if (religion.color)
	{
		output << "\t\tcolor " << *religion.color << "\n";
	}
	output << "\t\ticon =  " << religion.icon << "\n";
	if (!religion.allowedConversion.empty())
	{
		output << "\t\tallowed_conversion = {\n " << religion.allowedConversion << "}\n";
	}
	if (!religion.country.empty())
	{
		output << "\t\tcountry = {\n " << religion.country << "}\n";
	}
	if (!religion.countrySecondary.empty())
	{
		output << "\t\tcountry_as_secondary = {\n " << religion.countrySecondary << "}\n";
	}
	if (!religion.province.empty())
	{
		output << "\t\tprovince = {\n " << religion.province << "}\n";
	}
	if (!religion.unique.empty())
	{
		output << "\t\t" << religion.unique << "\n";
	}
	if (!religion.nonUnique.empty())
	{
		output << "\t\t" << religion.nonUnique << "\n";
	}
	if (!religion.staticBlob.empty())
	{
		output << "\t\t" << religion.staticBlob << "\n";
	}
	output << "\t}\n";
	output << "}\n";
	output << "\n";
	return output;
}