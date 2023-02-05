#ifndef TITLE_TAG_MAPPER_H
#define TITLE_TAG_MAPPER_H

#include "Parser.h"
#include "TitleTagMapping.h"

namespace mappers
{
class TitleTagMapper: commonItems::parser
{
  public:
	TitleTagMapper();
	explicit TitleTagMapper(std::istream& theStream); // testing
	void registerTitle(const std::string& ck3title, const std::string& eu4tag);

	std::optional<std::string> getTagForTitle(const std::string& ck3Title, int eu4Capital);
	std::optional<std::string> getTagForTitle(const std::string& ck3Title);

  private:
	void registerKeys();
	std::string generateNewTag();

	std::vector<TitleTagMapping> theMappings;
	std::map<std::string, std::string> registeredTitleTags; // We store already mapped countries here.
	std::set<std::string> usedTags;

	char generatedEU4TagPrefix = 'Z';
	char generatedEU4TagMidfix = '0';
	char generatedEU4TagSuffix = '0';
};
} // namespace mappers

#endif // TITLE_TAG_MAPPER_H