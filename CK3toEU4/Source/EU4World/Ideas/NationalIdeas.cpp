#include "NationalIdeas.h"
#include "Log.h"

// Will be used to load in defaults

EU4::NationalIdeas::NationalIdeas(std::shared_ptr<CK3::Culture> culture, const mappers::DynamicIdeasMapper& dynIdeasMapper):
	 culture(culture), traditionIdeas(culture->getTraditions()), ethosEffects(dynIdeasMapper.getEthosMap().at(culture->getEthos()))
{
	// Culture localized name is an optional value, so just forcing it into a normal string and given default value.
	if (auto possibleLocalizedName = culture->getLocalizedName(); possibleLocalizedName)
		localizedName = possibleLocalizedName.value();
	else
		localizedName = getDynamicName();

	// Default behavior for when cultures have less than 8 traditions. Good enough for now
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

		if (!changedEthos && replacee.find("ethos") != std::string::npos && rule.testRule(culture))
		{
			changedEthos = true;
			ethosEffects = rule.getNewEffect();
		}
		else if (const auto& it = std::find(tiBegin, tiEnd, replacee); it != tiEnd && rule.testRule(culture))
			*it = rule.getReplacement();
	}

	const auto& traditionMap = dynIdeasMapper.getTraditionMap();

	for (const auto& tradition: traditionIdeas)
		if (const auto& effectItr = traditionMap.find(tradition); effectItr == traditionMap.end())
		{
			Log(LogLevel::Warning) << "Tradition: " + tradition + " has no mapping in configuration/tradition_ideas.txt. Ideas based on " + tradition +
													" will have no effects. Consider adding a new link.";
			const mappers::AssignmentPair& errorPair = mappers::AssignmentPair();
			traditionEffects.push_back(std::vector<mappers::AssignmentPair>{errorPair});
		}
		else
			traditionEffects.push_back(traditionMap.at(tradition));
}