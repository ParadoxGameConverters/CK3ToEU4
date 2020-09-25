#include "ReligionDefinitionMapper.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::ReligionDefinitionMapper::ReligionDefinitionMapper()
{
	LOG(LogLevel::Info) << "-> Parsing religion definitions.";
	registerKeys();
	parseFile("configurables/religions/dynamic/next_icon.txt");
	parseFile("configurables/religions/dynamic/doctrine_definitions.txt");
	parseFile("configurables/religions/dynamic/religion_globals.txt");
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
	registerKeyword("current_icons", [this](const std::string& definition, std::istream& theStream) {
		currentIcon = commonItems::singleInt(theStream).getInt();
	});
	registerRegex(commonItems::catchallRegex, [this](const std::string& definition, std::istream& theStream) {
		mappings.insert(std::make_pair(definition, ReligionDefinitionMapping(theStream)));
	});
}

std::optional<mappers::ReligionDefinitionMapping> mappers::ReligionDefinitionMapper::getDefinition(const std::string& definition) const
{
	const auto& mapping = mappings.find(definition);
	if (mapping != mappings.end())
		return mapping->second;
	return std::nullopt;
}
