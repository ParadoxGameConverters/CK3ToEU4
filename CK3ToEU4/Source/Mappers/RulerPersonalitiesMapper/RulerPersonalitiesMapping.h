#ifndef RULER_PERSONALITIES_MAPPING_H
#define RULER_PERSONALITIES_MAPPING_H

#include "Parser.h"
#include <set>

namespace mappers
{
class RulerPersonalitiesMapping: commonItems::parser
{
  public:
	RulerPersonalitiesMapping() = default;
	explicit RulerPersonalitiesMapping(std::istream& theStream);

	[[nodiscard]] int evaluatePersonality(const std::set<std::string>& ck3Traits) const;

  private:
	void registerKeys();

	std::map<std::string, int> traits;
};
} // namespace mappers

#endif // RULER_PERSONALITIES_MAPPING_H