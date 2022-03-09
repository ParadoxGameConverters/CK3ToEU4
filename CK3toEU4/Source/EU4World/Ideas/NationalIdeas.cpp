#include "NationalIdeas.h"
#include <Log.h>

// Will be used to load in defaults

EU4::NationalIdeas::NationalIdeas(std::vector<std::string> tagsVector, CK3::Culture& culture, mappers::DynamicIdeasMapper& dynIdeasMapper): dynIdeasMapper(dynIdeasMapper)
{
	ethos = culture.getEthos();
	name = culture.getName();
	traditions = culture.getTraditions();
	tags = tagsVector;

	std::map<std::string, std::vector<std::string>> defaults = dynIdeasMapper.getDefaultMap();
	std::map<std::string, std::vector<std::string>>::iterator iterPair;

	iterPair = defaults.begin();

	while (traditions.size() < 8 && iterPair != defaults.end())
	{
		traditions.push_back(iterPair->first);
		iterPair++;
	}
}
