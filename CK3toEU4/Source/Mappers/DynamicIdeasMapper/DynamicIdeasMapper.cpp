#include "DynamicIdeasMapper.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::DynamicIdeasMapper::DynamicIdeasMapper()
{
	registerKeys();
	parseFile("configurables/tradition_ideas.txt");
	clearRegisteredKeywords();
}

mappers::DynamicIdeasMapper::DynamicIdeasMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::DynamicIdeasMapper::registerKeys()
{
	registerKeyword("ethos", [this](std::istream& theStream) {
		std::vector<std::string> ethosAndEffects = commonItems::getStrings(theStream); // Separates the ethos/tradition from their effects via head/tail op
		ethosMap.emplace(ethosAndEffects.front(), std::vector<std::string>(ethosAndEffects.begin() + 1, ethosAndEffects.end()));
	});
	registerKeyword("tradition", [this](std::istream& theStream) {
		std::vector<std::string> traditionAndEffects = commonItems::getStrings(theStream);
		traditionMap.emplace(traditionAndEffects.front(), std::vector<std::string>(traditionAndEffects.begin() + 1, traditionAndEffects.end()));
	});
	registerKeyword("default", [this](std::istream& theStream) {
		std::vector<std::string> defaultAndEffects = commonItems::getStrings(theStream);
		defaultMap.emplace(defaultAndEffects.front(), std::vector<std::string>(defaultAndEffects.begin() + 1, defaultAndEffects.end()));
		traditionMap.emplace(defaultAndEffects.front(), std::vector<std::string>(defaultAndEffects.begin() + 1, defaultAndEffects.end()));
	});
	registerKeyword("link", [this](std::istream& theStream) {
		DynamicIdeasMapper(theStream);
	});
	registerKeyword("eu4", [this](std::istream& theStream) {
		DynamicIdeasMapper(theStream);
	});
	registerKeyword("ck3", [this](std::istream& theStream) {
		DynamicIdeasMapper(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}