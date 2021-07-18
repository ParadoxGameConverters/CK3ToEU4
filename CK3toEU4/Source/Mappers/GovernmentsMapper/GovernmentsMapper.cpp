#include "GovernmentsMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::GovernmentsMapper::GovernmentsMapper()
{
	LOG(LogLevel::Info) << "-> Parsing government mappings.";
	registerKeys();
	parseFile("configurables/government_map.txt");
	clearRegisteredKeywords();
	LOG(LogLevel::Info) << "<> Loaded " << govMappings.size() << " governmental links.";
}

mappers::GovernmentsMapper::GovernmentsMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::GovernmentsMapper::registerKeys()
{
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream) {
		const GovernmentsMapping newMapping(theStream);
		govMappings.emplace_back(newMapping);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::pair<std::string, std::string>> mappers::GovernmentsMapper::matchGovernment(const std::string& ck3Government,
	 const std::string& ck3Title) const
{
	std::pair<std::string, std::string> toReturn;

	// first iterate over those that have a ck3title field, they take priority.
	for (const auto& mapping: govMappings)
	{
		if (mapping.getCK3Title().empty())
			continue;
		if (mapping.matchGovernment(ck3Government, ck3Title))
		{
			toReturn.first = mapping.getGovernment();
			toReturn.second = mapping.getReform();
			return toReturn;
		}
	}

	// Then might as well retry.
	for (const auto& mapping: govMappings)
		if (mapping.matchGovernment(ck3Government, ck3Title))
		{
			toReturn.first = mapping.getGovernment();
			toReturn.second = mapping.getReform();
			return toReturn;
		}
	return std::nullopt;
}
