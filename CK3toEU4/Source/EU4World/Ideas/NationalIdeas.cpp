#include "NationalIdeas.h"
#include <Log.h>

// Will be used to load in defaults

EU4::NationalIdeas::NationalIdeas(std::shared_ptr<CK3::Culture> culture, mappers::DynamicIdeasMapper& dynIdeasMapper)
	: culture(culture), dynIdeasMapper(dynIdeasMapper)
{
	// Culture localized name is an optional value, so just forcing it into a normal string and given defualt value.
	if (culture->getLocalizedName())
		localizedName = culture->getLocalizedName().value();
	else
		localizedName = getDynamicName();

	// Defualt behavior for when cultures have less than 8 traditions. Good enough for now
	std::map<std::string, std::vector<std::string>> defaults = dynIdeasMapper.getDefaultMap();
	std::map<std::string, std::vector<std::string>>::iterator iterPair;

	iterPair = defaults.begin();

	while (traditionIdeas.size() < 8 && iterPair != defaults.end())
	{
		traditionIdeas.push_back(iterPair->first);
		iterPair++;
	}
}
