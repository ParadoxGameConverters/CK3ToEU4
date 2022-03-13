#include "outNationalIdeas.h"
#include "OSCompatibilityLayer.h"

std::ostream& EU4::operator<<(std::ostream& output, const NationalIdeas& idea)
{
	output << idea.getDynamicName() + "_ideas = {";

	// National Traditions
	output << "\n\tstart = {";
	for (auto& effect: idea.getEthosEffects())
		output << "\n\t\t " + effect.type + " = " + effect.value;
	output << "\n\t}";

	// Trigger
	output << "\n\ttrigger = {\n\t\tculture = " + idea.getDynamicName() + "\n\t}\n";
	output << "\tfree = yes\n\n";

	// First 7 traditions become ideas, last one becomes ambition
	for (int i = 0; i < idea.getTraditionIdeas().size() - 1; i++)
	{
		output << "\t" + idea.getTraditionIdeas()[i] + " = {";

		for (auto& effect: idea.getTraditionEffects()[i])
			output << "\n\t\t" + effect.type + " = " + effect.value;

		output << "\n\t}\n";
	}

	// National Ambition
	output << "\tbonus = {\n\t\t" + idea.getTraditionEffects().back().front().type + " = " + idea.getTraditionEffects().back().front().value + "\n\t}\n";
	output << "}\n\n";

	return output;
}
