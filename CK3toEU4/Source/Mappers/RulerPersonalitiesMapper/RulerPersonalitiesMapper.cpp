#include "RulerPersonalitiesMapper.h"
#include "../../CK3World/Characters/Character.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::RulerPersonalitiesMapper::RulerPersonalitiesMapper()
{
	LOG(LogLevel::Info) << "-> Parsing Ruler Personalities";
	registerKeys();
	parseFile("configurables/ruler_personalities.txt");
	clearRegisteredKeywords();
	LOG(LogLevel::Info) << "<> " << theMappings.size() << " personalities loaded.";
}

mappers::RulerPersonalitiesMapper::RulerPersonalitiesMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::RulerPersonalitiesMapper::registerKeys()
{
	registerRegex("[a-zA-Z0-9_-]+", [this](const std::string& personality, std::istream& theStream) {
		const auto newMapping = RulerPersonalitiesMapping(theStream);
		theMappings.insert(std::pair(personality, newMapping));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::set<std::string> mappers::RulerPersonalitiesMapper::evaluatePersonalities(const std::pair<int, std::shared_ptr<CK3::Character>>& theCharacter) const
{
	// In CK2 they are traits. In EU4 they are personalities.
	const auto& incTraits = theCharacter.second->getTraits();
	std::set<std::string> ck2Traits;
	for (const auto& trait: incTraits)
		ck2Traits.insert(trait.second);

	std::vector<std::pair<int, std::string>> scoreBoard; // personalityscore, personality
	std::set<std::string> toReturn;

	for (const auto& mapping: theMappings)
	{
		auto score = mapping.second.evaluatePersonality(ck2Traits);
		scoreBoard.emplace_back(std::pair(score, mapping.first));
	}
	std::sort(scoreBoard.rbegin(), scoreBoard.rend());

	// send back first two. EU4 should deal with excess.
	for (auto i = 0; i < std::min(2, static_cast<int>(theMappings.size())); i++)
	{
		toReturn.insert(scoreBoard[i].second);
	}
	return toReturn;
}
