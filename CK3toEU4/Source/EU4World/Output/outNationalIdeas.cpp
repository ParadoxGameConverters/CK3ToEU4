#include "outNationalIdeas.h"
#include "OSCompatibilityLayer.h"

std::ostream& EU4::operator<<(std::ostream& output, const NationalIdeas& idea)
{
	auto& ethosMap = idea.getMapper().getEthosMap();
	auto& traditionMap = idea.getMapper().getTraditionMap();

	auto& ethos = idea.getEthos();

	std::vector<std::string> traditions = idea.getTraditionIdeas();

	output << idea.getDynamicName() + "_ideas = {\n";

	auto& ethosEffects = ethosMap.at(ethos);

	// National Traditions
	output << "\tstart = {\n\t\t" + ethosEffects[0].first + " = " + ethosEffects[0].second + "\n\t\t"
		+ ethosEffects[1].first + " + " + ethosEffects[1].second + "\n\t}\n";

	// Trigger
	output << "\n\ttrigger = {\n\t\tculture = " + idea.getDynamicName() + "\n\t}\n";

	output << "\tfree = yes\n\n";

	// First 7 traditions become ideas, last one becomes ambition
	std::vector<std::string>::iterator iterTraditions = traditions.begin();

	while (iterTraditions != traditions.end() - 1)
	{
		output << "\t" + *iterTraditions + " = {";

		auto& effects = traditionMap.at(*iterTraditions);
		for (auto& effectPair: effects)
			output << "\n\t\t" + effectPair.first + " = " + effectPair.second;

		output << "\n\t}\n";

		iterTraditions++;
	}

	output << "\tbonus = {\n\t\t" + traditionMap.at(traditions.back())[0].first + " = " + traditionMap.at(traditions.back())[0].second + "\n\t}\n";
	output << "}\n\n";

	return output;
}
