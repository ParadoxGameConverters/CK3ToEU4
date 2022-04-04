#include "DynamicIdeasLink.h"
#include "CommonRegexes.h"
#include "DynamicIdeasRuleEnum.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::DynamicIdeasLink::DynamicIdeasLink(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::DynamicIdeasLink::registerKeys()
{
	registerKeyword("ethos", [this](std::istream& theStream) {
		ethos = commonItems::getString(theStream);
	});
	registerKeyword("tradition", [this](std::istream& theStream) {
		tradition = commonItems::getString(theStream);
	});
	registerKeyword("default", [this](std::istream& theStream) {
		defaultString = commonItems::getString(theStream);
	});
	registerKeyword("idea_name", [this](std::istream& theStream) {
		ideaName = commonItems::getString(theStream);
	});
	registerKeyword("ck3", [this](std::istream& theStream) {
		const auto scraper = DynamicIdeasLink(theStream);
		if (auto possibleEthos = scraper.getEthos(); possibleEthos)
			ethos = possibleEthos;
		else if (auto possibleTradition = scraper.getTradition(); possibleTradition)
			tradition = possibleTradition;
		else if (auto possibleDefault = scraper.getDefault(); possibleDefault)
			defaultString = possibleDefault;
		else
			Log(LogLevel::Info) << "Missing ck3 value in link in tradition_ideas.txt";
	});
	registerKeyword("eu4", [this](std::istream& theStream) {
		const auto scraper = DynamicIdeasLink(theStream);
		effects = scraper.getEffects();
	});
	registerKeyword("rule", [this](std::istream& theStream) {
		const auto scraper = commonItems::assignments(theStream).getAssignments();
		for (const auto& [type, value]: scraper)
		{
			if (type == "idea_name")
				ideaName = value;
			else
				rules.push_back({StringToRuleType.at(type), value}); // Convert std::pair to RulePair
		}
	});
	registerRegex(R"(\w+)", [this](const std::string& effect, std::istream& theStream) {
		EffectPair pair = {effect, commonItems::getString(theStream)};
		effects.push_back(pair);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}