#include "DevWeightsMapper.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::DevWeightsMapper::DevWeightsMapper()
{
	registerKeys();
	parseFile("configurables/dev_weights.txt");
	clearRegisteredKeywords();
}

mappers::DevWeightsMapper::DevWeightsMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::DevWeightsMapper::registerKeys()
{
	registerKeyword("dev_from_building", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleDouble devDbl(theStream);
		devFromBuilding = devDbl.getDouble();
	});
	registerKeyword("dev_from_holding", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleDouble devDbl(theStream);
		devFromHolding = devDbl.getDouble();
	});
	registerKeyword("dev_from_development", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleDouble devDbl(theStream);
		devFromDev = devDbl.getDouble();
	});
	registerKeyword("dev_treshold", [this](std::istream& theStream) {
		devTreshold = commonItems::getDouble(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}