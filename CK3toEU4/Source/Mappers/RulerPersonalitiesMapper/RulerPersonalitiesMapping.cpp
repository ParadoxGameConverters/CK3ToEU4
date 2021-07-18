#include "RulerPersonalitiesMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::RulerPersonalitiesMapping::RulerPersonalitiesMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::RulerPersonalitiesMapping::registerKeys()
{
	registerRegex("[a-zA-Z0-9_-]+", [this](const std::string& personality, std::istream& theStream) {
		const commonItems::singleInt valueInt(theStream);
		traits.insert(std::pair(personality, valueInt.getInt()));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

int mappers::RulerPersonalitiesMapping::evaluatePersonality(const std::set<std::string>& ck3Traits) const
{
	// In CK3 they are traits. In EU4 they are personalities.
	auto score = 0;
	for (const auto& trait: traits)
		if (ck3Traits.count(trait.first))
			score += trait.second;

	return score;
}
