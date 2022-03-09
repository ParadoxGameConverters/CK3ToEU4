#include "outNationalIdeas.h"
#include "OSCompatibilityLayer.h"

std::ostream& EU4::operator<<(std::ostream& output, const NationalIdeas& idea)
{
	auto& ethosMap = idea.getMapper().getEthosMap();
	auto& traditionMap = idea.getMapper().getTraditionMap();
	auto& ethos = idea.getEthos();
	std::vector<std::string> traditions = idea.getTraditions();

	output << idea.getName() + "_ideas = {\n";

	// National Traditions
	output << "\tstart = {\n\t\t" + ethosMap.at(ethos)[0] + " = " + ethosMap.at(ethos)[1] + 
		"\n\t\t" + ethosMap.at(ethos)[2] + " + " + ethosMap.at(ethos)[3] + "\n\t}\n";
	// Trigger
	output << "\n\ttrigger = {\n\t\tculture = " + idea.getName() + "\n\t}\n";
	output << "free = yes\n\n";

	// First 7 traditions become ideas, last one becomes ambition
	std::vector<std::string>::iterator iterTraditions = traditions.begin();

	while (iterTraditions != traditions.end() - 1)
	{
		output << "\t" + *iterTraditions + " = {";
		std::vector<std::string> effects = traditionMap.at(*iterTraditions);

		for (int i = 0; i < effects.size(); i++)
		{
			if (i % 2 == 0)
				output << "\n\t\t" + effects[i];
			else
				output << " = " + effects[i];
		}

		output << "\n\t}\n";

		iterTraditions++;
	}

	output << "\tbonus = {\n\t\t" + traditionMap.at(traditions.back())[0] + " = " + traditionMap.at(traditions.back())[1] + "\n\t}\n";
	output << "}\n\n";

	return output;
}
