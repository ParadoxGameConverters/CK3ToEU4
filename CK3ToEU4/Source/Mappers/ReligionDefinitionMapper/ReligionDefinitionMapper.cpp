#include "ReligionDefinitionMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::ReligionDefinitionMapper::ReligionDefinitionMapper()
{
	Log(LogLevel::Info) << "-> Parsing religion definitions.";
	registerKeys();
	parseFile(std::filesystem::path("configurables/religions/dynamic/current_icons.txt"));
	parseFile(std::filesystem::path("configurables/religions/dynamic/doctrine_definitions.txt"));
	clearRegisteredKeywords();
	registerStaticKeys();
	parseFile(std::filesystem::path("configurables/religions/dynamic/religion_globals.txt"));
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << mappings.size() << " religious intricacies.";
}

mappers::ReligionDefinitionMapper::ReligionDefinitionMapper(std::istream& theStream1, std::istream& theStream2)
{
	registerKeys();
	parseStream(theStream1);
	clearRegisteredKeywords();
	registerStaticKeys();
	parseStream(theStream2);
	clearRegisteredKeywords();
}

void mappers::ReligionDefinitionMapper::registerKeys()
{
	registerKeyword("current_icons", [this](const std::string& definition, std::istream& theStream) {
		const auto icon = commonItems::singleInt(theStream).getInt();
		currentIcon = icon;
		originalIcons = icon;
	});
	registerRegex(commonItems::catchallRegex, [this](const std::string& definition, std::istream& theStream) {
		const auto definitionBlobs = ReligionDefinitionMapping(theStream);
		mappings.insert(std::make_pair(definition, definitionBlobs));
	});
}

void mappers::ReligionDefinitionMapper::registerStaticKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& religionName, std::istream& theStream) {
		const auto blob = commonItems::stringOfItem(theStream).getString();
		if (blob.size() > 4)
			staticStorage.insert(std::make_pair(religionName, blob.substr(3, blob.size() - 4)));
		else
			staticStorage.clear();
	});
}

std::optional<mappers::ReligionDefinitionMapping> mappers::ReligionDefinitionMapper::getDefinition(const std::string& definition) const
{
	const auto& mapping = mappings.find(definition);
	if (mapping != mappings.end())
		return mapping->second;
	return std::nullopt;
}

std::optional<std::string> mappers::ReligionDefinitionMapper::getStaticBlob(const std::string& religion) const
{
	const auto& mapping = staticStorage.find(religion);
	if (mapping != staticStorage.end())
		return mapping->second;
	return std::nullopt;
}
