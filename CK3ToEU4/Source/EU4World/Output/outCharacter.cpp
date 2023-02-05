#include "outCharacter.h"
#include "OSCompatibilityLayer.h"

std::ostream& EU4::operator<<(std::ostream& output, const Character& character)
{
	output << "\t\tname = \"" << commonItems::convertUTF8ToWin1252(character.name) << "\"\n";
	if (!character.monarchName.empty())
		output << "\t\tmonarch_name = \"" << commonItems::convertUTF8ToWin1252(character.monarchName) << "\"\n";
	if (!character.dynasty.empty())
	{
		output << "\t\tdynasty = \"" << commonItems::convertUTF8ToWin1252(character.dynasty) << "\"\n";
	}
	if (!character.id)
		output << "\t\tadm = " << character.adm << "\n";
	if (!character.id)
		output << "\t\tdip = " << character.dip << "\n";
	if (!character.id)
		output << "\t\tmil = " << character.mil << "\n";
	if (!character.originCountry.empty())
		output << "\t\tcountry_of_origin = " << character.originCountry << "\n";
	if (!character.id)
		output << "\t\tbirth_date = " << character.birthDate << "\n";
	if (character.regency)
		output << "\t\tregent = yes\n";
	if (character.deathDate != date("1.1.1"))
		output << "\t\tdeath_date = " << character.deathDate << "\n";
	if (character.female)
		output << "\t\tfemale = yes\n";
	if (character.claim)
		output << "\t\tclaim = " << character.claim << "\n";
	if (!character.religion.empty())
		output << "\t\treligion = " << character.religion << "\n";
	if (!character.culture.empty())
		output << "\t\tculture = " << character.culture << "\n";
	if (character.id)
		output << "\t\tadvisor_id = " << character.id << "\n";
	if (character.location)
		output << "\t\tlocation = " << character.location << "\n";
	if (character.skill)
		output << "\t\tskill = " << character.skill << "\n";
	if (!character.type.empty())
		output << "\t\ttype = " << character.type << "\n";
	if (character.discount)
		output << "\t\tdiscount = yes\n";
	if (character.appearDate != date("1.1.1"))
		output << "\t\tdate = " << character.appearDate << "\n";
	return output;
}
