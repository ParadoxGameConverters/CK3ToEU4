#ifndef RULER_PERSONALITY_MAPPER_H
#define RULER_PERSONALITY_MAPPER_H

#include "RulerPersonalitiesMapping.h"
#include "Parser.h"
#include <set>

namespace CK3
{
class Character;
}

namespace mappers
{
class RulerPersonalitiesMapper: commonItems::parser
{
  public:
	RulerPersonalitiesMapper();
	explicit RulerPersonalitiesMapper(std::istream& theStream);

	[[nodiscard]] std::set<std::string> evaluatePersonalities(const std::shared_ptr<CK3::Character>& theCharacter) const;
	[[nodiscard]] const auto& getMappings() const { return theMappings; } // used for testing

  private:
	void registerKeys();

	std::map<std::string, RulerPersonalitiesMapping> theMappings;
};
} // namespace mappers

#endif // RULER_PERSONALITY_MAPPER_H