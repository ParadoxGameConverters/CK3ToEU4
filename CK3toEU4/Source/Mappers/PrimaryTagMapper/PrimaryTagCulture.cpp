#include "PrimaryTagCulture.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::PrimaryTagCulture::PrimaryTagCulture(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::PrimaryTagCulture::registerKeys()
{
	registerKeyword("primary", [this](const std::string& unused, std::istream& theStream) {
		tag = commonItems::singleString(theStream).getString();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::PrimaryTagCulture::getTag() const
{
	if (!tag.empty())
		return tag;
	return std::nullopt;
}
