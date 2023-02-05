#ifndef TITLE_TAG_MAPPING_H
#define TITLE_TAG_MAPPING_H

#include "Parser.h"
#include <set>

namespace mappers
{
class TitleTagMapping: commonItems::parser
{
  public:
	TitleTagMapping() = default;
	explicit TitleTagMapping(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> titleMatch(const std::string& ck3title) const;
	[[nodiscard]] std::optional<std::string> capitalMatch(int eu4capital) const;
	[[nodiscard]] bool fallbackMatch() const;

  private:
	void registerKeys();

	std::string eu4Tag;
	std::string ck3Title;
	std::set<int> capitals;
	bool fallback = false;
};
} // namespace mappers

#endif // TITLE_TAG_MAPPING_H