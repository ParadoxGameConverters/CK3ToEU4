#include "TitleTagMapping.h"
#include "ParserHelpers.h"

mappers::TitleTagMapping::TitleTagMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::TitleTagMapping::registerKeys()
{
	registerKeyword("eu4", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString tagStr(theStream);
		eu4Tag = tagStr.getString();
	});
	registerKeyword("ck3", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString titleStr(theStream);
		ck3Title = titleStr.getString();
	});
	registerKeyword("fallback", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::singleString fallbackStr(theStream);
		fallback = fallbackStr.getString() == "yes";
	});
	registerKeyword("capitals", [this](const std::string& unused, std::istream& theStream) {
		const commonItems::intList capList(theStream);
		const auto& theList = capList.getInts();
		capitals.insert(theList.begin(), theList.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::TitleTagMapping::titleMatch(const std::string& ck3title) const
{
	if (ck3Title == ck3title)
		return eu4Tag;
	return std::nullopt;
}

std::optional<std::string> mappers::TitleTagMapping::capitalMatch(int eu4capital) const
{
	if (capitals.count(eu4capital))
		return eu4Tag;
	return std::nullopt;
}

bool mappers::TitleTagMapping::fallbackMatch() const
{
	if (eu4Tag.empty())
		return false;
	return fallback;
}
