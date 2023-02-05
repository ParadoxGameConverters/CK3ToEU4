#include "outNationalIdeas.h"
#include "OSCompatibilityLayer.h"

std::ostream& EU4::operator<<(std::ostream& output, const NationalIdeas& idea)
{
	output << idea.getDynamicName() + "_ideas = {";

	// National Traditions
	output << "\n\tstart = {";
	for (auto& effect: idea.getEthosEffects())
		output << "\n\t\t " + effect.modifier + " = " + effect.value;
	output << "\n\t}";

	// Trigger
	output << "\n\ttrigger = {\n\t\tculture = " + idea.getDynamicName() + "\n\t}\n";
	output << "\tfree = yes\n\n";

	// First 7 traditions become ideas, last one becomes ambition
	const auto& traditionIdeas = idea.getTraditionIdeas();
	const auto& traditionEffects = idea.getTraditionEffects();

	for (int i = 0; i < traditionIdeas.size() - 1; i++)
	{
		output << "\t" + traditionIdeas[i] + " = {";

		for (auto& effect: traditionEffects[i])
			output << "\n\t\t" + effect.modifier + " = " + effect.value;

		output << "\n\t}\n";
	}

	// National Ambition
	const auto& ambitionEffect = traditionEffects[7].front(); // For multi effect ideas, the ambition is only the first effect, not all effects
	output << "\tbonus = {\n\t\t" + ambitionEffect.modifier + " = " + ambitionEffect.value + "\n\t}\n";
	output << "}\n\n";

	return output;
}