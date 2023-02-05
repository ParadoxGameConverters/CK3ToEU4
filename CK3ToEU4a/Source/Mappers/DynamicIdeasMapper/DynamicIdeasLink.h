#ifndef DYNAMIC_IDEAS_LINK
#define DYNAMIC_IDEAS_LINK

#include "AssignmentPairs.h"
#include "Parser.h"

namespace mappers
{

class DynamicIdeasLink: commonItems::parser
{
  public:
	DynamicIdeasLink() = default;
	explicit DynamicIdeasLink(std::istream& theStream);

	[[nodiscard]] const auto& getEthos() const { return ethos; }
	[[nodiscard]] const auto& getTradition() const { return tradition; }
	[[nodiscard]] const auto& getDefault() const { return defaultString; }
	[[nodiscard]] const auto& getEffects() const { return effects; }
	[[nodiscard]] const auto& getIdeaName() const { return ideaName; }
	[[nodiscard]] const auto& getRules() const { return rules; }

  private:
	void registerKeys();

	std::optional<std::string> ethos;
	std::optional<std::string> tradition;
	std::optional<std::string> defaultString;
	std::optional<std::string> ideaName;
	std::vector<EffectPair> effects; // (modifier = "global_manpower_modifier", value = "0.10")
	std::vector<RulePair> rules;		// (rule = RULE_TYPE::HERITAGE, value = "heritage_north_germanic")
};
} // namespace mappers
#endif // DYNAMIC_IDEAS_LINK