#include "Area.h"
#include "ParserHelpers.h"

mappers::Area::Area(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::Area::registerKeys()
{
	registerKeyword("color", commonItems::ignoreItem);
	registerRegex("[0-9]+", [this](const std::string& number, std::istream& theStream) {
		// This is a peculiar file format where we pull free-floating numbers from thin air
		provinces.insert(std::pair(std::stoi(number), nullptr));
	});
}

bool mappers::Area::areaContainsProvince(const int province) const
{
	if (provinces.count(province))
		return true;
	return false;
}
