#include "PrimaryTagCultureGroup.h"
#include "PrimaryTagMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "OSCompatibilityLayer.h"

void mappers::PrimaryTagMapper::loadPrimaryTags(const Configuration& theConfiguration)
{
	LOG(LogLevel::Info) << "-> Sifting Through EU4 Cultures";
	registerKeys();

	for (const auto& filename: Utils::GetAllFilesInFolder("blankMod/output/common/cultures/"))
		parseFile("blankMod/output/common/cultures/" + filename);
	for (const auto& filename: Utils::GetAllFilesInFolder(theConfiguration.getEU4Path() + "/common/cultures/"))
		parseFile(theConfiguration.getEU4Path() + "/common/cultures/" + filename);
	
	clearRegisteredKeywords();
	LOG(LogLevel::Info) << "<> " << cultureTags.size() << " culture tags located.";
}

mappers::PrimaryTagMapper::PrimaryTagMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::PrimaryTagMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& unused, std::istream& theStream) {
		const PrimaryTagCultureGroup theGroup(theStream);
		const auto& collectedTags = theGroup.getCollectedTags();
		cultureTags.insert(collectedTags.begin(), collectedTags.end());		
	});
}

std::optional<std::string> mappers::PrimaryTagMapper::getPrimaryTagForCulture(const std::string& culture) const
{
	const auto& cultureItr = cultureTags.find(culture);
	if (cultureItr != cultureTags.end())
		return cultureItr->second;
	return std::nullopt;
}
