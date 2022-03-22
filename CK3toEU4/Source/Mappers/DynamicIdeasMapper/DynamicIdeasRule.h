#ifndef DYNAMIC_IDEAS_RULE
#define DYNAMIC_IDEAS_RULE

#include "../../CK3World/Cultures/Culture.h"
#include "DynamicIdeasLink.h"
#include <set>

// Just a helper fxn. Does it get it's own namespace? Should it be in mappers:: ? Put it at the top of the cpp file?
std::string getLeadStr(const std::string& str);

namespace mappers
{

enum class RULE_TYPE
{
	HERITAGE,
	ETHOS_TRIGGER
};

static std::map<RULE_TYPE, int> RuleTypeToPrecedence{{RULE_TYPE::HERITAGE, 0}, {RULE_TYPE::ETHOS_TRIGGER, 1}};
static std::map<std::string, RULE_TYPE> StringToRuleType{{"heritage", RULE_TYPE::HERITAGE}, {"ethos_rule", RULE_TYPE::ETHOS_TRIGGER}};

class DynamicIdeasRule
{
  public:
	DynamicIdeasRule() = default;
	DynamicIdeasRule(std::vector<AssignmentPair> ruleInfo, std::string replacee, std::vector<AssignmentPair> newEffect);

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
	std::string replacementIdentifier; // tradition_fisherman_heritage_north_germanic
	std::vector<AssignmentPair> newEffect;
	std::set<int> precedenceLevel;
	std::vector<AssignmentPair> ruleInfo;
};

std::ostream& operator<<(std::ostream& output, const DynamicIdeasRule& rule);

} // namespace mappers
#endif // DYNAMIC_IDEAS_RULE