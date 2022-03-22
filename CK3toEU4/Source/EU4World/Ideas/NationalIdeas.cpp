#include "NationalIdeas.h"

// Will be used to load in defaults

EU4::NationalIdeas::NationalIdeas(std::shared_ptr<CK3::Culture> culture, const mappers::DynamicIdeasMapper& dynIdeasMapper):
	 culture(culture), traditionIdeas(culture->getTraditions()), ethosEffects(dynIdeasMapper.getEthosMap().at(culture->getEthos()))
{
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
		++iter;
	}

	// Prepare rules before base mapping
	const auto& tiBegin = traditionIdeas.begin();
	const auto& tiEnd = traditionIdeas.end();
	const auto& rules = dynIdeasMapper.getRules();
	bool changedEthos = false;

	for (const auto& rule: rules)
	{
		const auto& replacee = rule.getReplacee();

		if (!changedEthos && replacee.contains("ethos") && rule.testRule(culture))
		{
			changedEthos = true;
			ethosEffects = rule.getNewEffect();
		}
		else if (const auto& it = std::find(tiBegin, tiEnd, replacee); it != tiEnd && rule.testRule(culture))
			*it = rule.getReplacement();
	}

	for (const auto& tradition: traditionIdeas)
		traditionEffects.push_back(dynIdeasMapper.getTraditionMap().at(tradition));
}