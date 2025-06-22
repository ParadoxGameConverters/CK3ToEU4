#include "PrimaryTagMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "PrimaryTagCultureGroup.h"

void mappers::PrimaryTagMapper::loadPrimaryTags(const Configuration& theConfiguration)
{
	registerKeys();

	for (const auto& filename: commonItems::GetAllFilesInFolder("blankMod/output/common/cultures/"))
		parseFile("blankMod/output/common/cultures" / filename);
	for (const auto& filename: commonItems::GetAllFilesInFolder(theConfiguration.getEU4Path() / "common/cultures/"))
		parseFile(theConfiguration.getEU4Path() / "common/cultures" / filename);

	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << cultureTags.size() << " culture tags located.";
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
