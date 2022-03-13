#include "DynamicIdeasLink.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include "Log.h"

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
	registerKeyword("ck3", [this](std::istream& theStream) {
		const auto scraper = DynamicIdeasLink(theStream);
		if (auto possibleEthos = scraper.getEthos(); possibleEthos)
			ethos = possibleEthos;
		else if (auto possibleTradition = scraper.getTradition(); possibleTradition)
			tradition = possibleTradition;
		else if (auto possibleDefault = scraper.getDefault(); possibleDefault)
			defaultString = possibleDefault;
		else
			Log(LogLevel::Info) << "Missing value in tradition_ideas.txt";
	});
	registerKeyword("eu4", [this](std::istream& theStream) {
		const auto scraper = DynamicIdeasLink(theStream);
		effects = scraper.getPair();
	});
	registerKeyword("modifier", [this](std::istream& theStream) {
		const auto scraper = DynamicIdeasLink(theStream);
		rules = scraper.getPair();
	});
	registerRegex(R"(\w+)", [this](const std::string& ruleOrEffect, std::istream& theStream) {
		ambiguosPairs.push_back(std::make_pair(ruleOrEffect, commonItems::getString(theStream)));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}