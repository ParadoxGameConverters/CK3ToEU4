#include "ReligionDefinitionMapper.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::ReligionDefinitionMapper::ReligionDefinitionMapper()
{
	LOG(LogLevel::Info) << "-> Parsing religion definitions.";
	registerKeys();
	parseFile("configurables/doctrine_definitions.txt");
	parseFile("configurables/religion_globals.txt");
	clearRegisteredKeywords();
	LOG(LogLevel::Info) << "<> Loaded " << mappings.size() << " religious intricacies.";
}

mappers::ReligionDefinitionMapper::ReligionDefinitionMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ReligionDefinitionMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& definition, std::istream& theStream) {
		mappings.insert(std::make_pair(definition, ReligionDefinitionMapping(theStream)));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<mappers::ReligionDefinitionMapping> mappers::ReligionDefinitionMapper::getDefinition(const std::string& definition) const
{
	const auto& mapping = mappings.find(definition);
	if (mapping != mappings.end())
		return mapping->second;
	return std::nullopt;
}
