#include "NationalIdeas.h"

// Will be used to load in defaults

EU4::NationalIdeas::NationalIdeas(CK3::Culture& culture, mappers::DynamicIdeasMapper& dynIdeasMapper)
{
	ethos = culture.getEthos();
	name = culture.getName();
	traditions = culture.getTraditions();

	std::map<std::string, std::vector<std::string>> defaults = dynIdeasMapper.getDefaultMap();
	std::map<std::string, std::vector<std::string>>::iterator iterPair;

	iterPair = defaults.begin();

	while (traditions.size() < 8 && iterPair != defaults.end())
	{
		traditions.push_back(iterPair->first);
		iterPair++;
	}
}
