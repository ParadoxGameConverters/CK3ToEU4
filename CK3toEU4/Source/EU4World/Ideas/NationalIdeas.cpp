#include "NationalIdeas.h"
#include <Log.h>

// Will be used to load in defaults

EU4::NationalIdeas::NationalIdeas(std::shared_ptr<CK3::Culture> culture, mappers::DynamicIdeasMapper& dynIdeasMapper): dynIdeasMapper(dynIdeasMapper)
{
	ethos = culture->getEthos();
	culturalName = culture->getName();
	traditions = culture->getTraditions();

	if (culture->getLocalizedName())
		localizedName = culture->getLocalizedName().value();
	else
		localizedName = culturalName;

	std::map<std::string, std::vector<std::string>> defaults = dynIdeasMapper.getDefaultMap();
	std::map<std::string, std::vector<std::string>>::iterator iterPair;

	iterPair = defaults.begin();

	while (traditions.size() < 8 && iterPair != defaults.end()) // Probably need to change
	{
		traditions.push_back(iterPair->first);
		iterPair++;
	}
}
