#ifndef DYNAMIC_IDEAS_RULE
#define DYNAMIC_IDEAS_RULE

#include "../../CK3World/Cultures/Culture.h"
#include "DynamicIdeasLink.h"
#include <set>


namespace mappers
{

class DynamicIdeasRule
{
  public:
	DynamicIdeasRule() = default;
	DynamicIdeasRule(const std::vector<RulePair>& ruleInfo,
		 const std::vector<EffectPair>& newEffect,
		 const std::string& replacee,
		 const std::optional<std::string> ideaName);

	[[nodiscard]] const auto& getReplacee() const { return replacee; }
	[[nodiscard]] const auto& getReplacement() const { return replacementIdentifier; }
	[[nodiscard]] const auto& getNewEffect() const { return newEffect; }
	[[nodiscard]] const auto& getPrecedence() const { return precedenceLevel; }

	bool testRule(const std::shared_ptr<CK3::Culture> culture) const;

	bool operator<(const DynamicIdeasRule& rhs) const;
	bool operator==(const DynamicIdeasRule& rhs) const;

  private:
	friend std::ostream& operator<<(std::ostream&, const DynamicIdeasRule&); // For debug warnings

	std::string replacee;				  // tradition_fisherman
	std::string replacementIdentifier; // tradition_fisherman__heritage_north_germanic
	std::vector<EffectPair> newEffect;
	std::set<int> precedenceLevel;
	std::vector<RulePair> ruleInfo;
};

std::ostream& operator<<(std::ostream& output, const DynamicIdeasRule& rule);

} // namespace mappers
#endif // DYNAMIC_IDEAS_RULE