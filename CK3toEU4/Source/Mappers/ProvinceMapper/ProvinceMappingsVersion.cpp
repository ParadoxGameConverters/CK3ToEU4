#include "ProvinceMappingsVersion.h"
#include "ParserHelpers.h"

mappers::ProvinceMappingsVersion::ProvinceMappingsVersion(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ProvinceMappingsVersion::registerKeys()
{
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream) {
		const auto newMapping = std::make_shared<ProvinceMapping>(theStream);
		if (newMapping->getCK3Provinces().empty() && newMapping->getEU4Provinces().empty())
			return;
		mappings.push_back(newMapping);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
