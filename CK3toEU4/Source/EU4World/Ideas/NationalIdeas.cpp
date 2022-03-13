#include "NationalIdeas.h"
#include <Log.h>

// Will be used to load in defaults

EU4::NationalIdeas::NationalIdeas(std::shared_ptr<CK3::Culture> culture, mappers::DynamicIdeasMapper& dynIdeasMapper)
	: culture(culture), dynIdeasMapper(dynIdeasMapper)
{
	traditionIdeas = culture->getTraditions();

	// Culture localized name is an optional value, so just forcing it into a normal string and given defualt value.
	if (auto possibleLocalizedName = culture->getLocalizedName(); possibleLocalizedName)
		localizedName = possibleLocalizedName.value();
	else
		localizedName = getDynamicName();

	// Defualt behavior for when cultures have less than 8 traditions. Good enough for now
	std::vector<std::string> defaults = dynIdeasMapper.getDefaults();
	std::vector<std::string>::iterator iter;

	iter = defaults.begin();

	while (traditionIdeas.size() < 8 && iter != defaults.end())
	{
		traditionIdeas.push_back(*iter);
		iter++;
	}
}
