#include "ProvinceMappingsVersion.h"
#include "ParserHelpers.h"

mappers::ProvinceMappingsVersion::ProvinceMappingsVersion(std::istream& theStream)
{
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream) {
		const ProvinceMapping newMapping(theStream);
		mappings.push_back(newMapping);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	parseStream(theStream);
	clearRegisteredKeywords();
}