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
	registerKeyword("ethos", [this](const std::string& unused, std::istream& theStream) {
		std::vector<std::string> ethosAndEffects = commonItems::getStrings(theStream); // Separates the ethos/tradition from their effects via head/tail op
		ethosMap.emplace(ethosAndEffects.front(), std::vector<std::string>(ethosAndEffects.begin() + 1, ethosAndEffects.end()));
	});
	registerKeyword("tradition", [this](const std::string& unused, std::istream& theStream) {
		std::vector<std::string> traditionAndEffects = commonItems::getStrings(theStream);
		ethosMap.emplace(traditionAndEffects.front(), std::vector<std::string>(traditionAndEffects.begin() + 1, traditionAndEffects.end()));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}