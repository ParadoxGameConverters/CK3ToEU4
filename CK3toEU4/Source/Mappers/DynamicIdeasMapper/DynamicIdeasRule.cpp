#include "DynamicIdeasRule.h"
#include "DynamicIdeasRuleEnum.h"
#include "OSCompatibilityLayer.h"

mappers::DynamicIdeasRule::DynamicIdeasRule(const std::vector<RulePair>& ruleInfo,
	 const std::vector<EffectPair>& newEffect,
	 const std::string& replacee,
	 const std::optional<std::string> ideaName):
	 ruleInfo(ruleInfo),
	 replacee(replacee), newEffect(newEffect), replacementIdentifier(replacee)
{
	if (ideaName)
		replacementIdentifier = ideaName.value();
	else
		replacementIdentifier += '_'; // Need extra _ to know when non-base tradition name starts

	for (const auto& rule: ruleInfo)
	{
		// Set precedance level
		precedenceLevel.emplace(RuleTypeToPrecedence.at(rule.rule_type));

		// Craft unique identfier name
		// Support coastal = yes and heritage = heritage_north_germanic while keeping as much brevity as possible
		if (!ideaName)
		{
			const auto& ruleName = RuleTypeToString.at(rule.rule_type);
			const std::string& prefix = getLeadStr(ruleName);
			if (rule.value.find(prefix) != std::string::npos)
				replacementIdentifier += ("_" + rule.value);
			else
				replacementIdentifier += ("_" + ruleName + "_" + rule.value);
		}
	}
}

bool mappers::DynamicIdeasRule::testRules(const std::shared_ptr<CK3::Culture> culture) const
{
	// Generate rule
	for (const auto& rule: ruleInfo)
	{
		// Shortcut return false if any rule doesn't pass, otherwise return true
		if (!testRule(rule.rule_type, rule.value, culture))
			return false;
	}
	return true;
}

bool mappers::DynamicIdeasRule::operator<(const DynamicIdeasRule& rhs) const
{
	// Allow a rule to be used more than once so long as it has a different target
	if (precedenceLevel == rhs.precedenceLevel)
	{
		if (ruleInfo == rhs.ruleInfo)
			return replacee != rhs.replacee;
		else
			return true;
	}

	// Actual Strict weak ordering
	// Desired ordering is that more specific rules are < (a.k.a higer precedence) less specifc rules when all else is equal.
	// In alpha terms this would equate to paradoxical coming before paradox.
	const auto& lhsStart = precedenceLevel.begin();
	const auto& lhsEnd = precedenceLevel.end();
	const auto& rhsStart = rhs.precedenceLevel.begin();
	const auto& rhsEnd = rhs.precedenceLevel.end();

	// Am I >= to the rhs rule? If so, my set difference would be empty.
	std::set<int> lComparator;
	std::set_difference(lhsStart, lhsEnd, rhsStart, rhsEnd, std::inserter(lComparator, lComparator.begin()));

	// If I'm empty, I'm either equal to, or a shorter version of rhs, either way I don't have precedence.
	if (lComparator.empty())
		return false;

	// Is rhs >= me? If so, its set difference would be empty.
	std::set<int> rComparator;
	std::set_difference(rhsStart, rhsEnd, lhsStart, lhsEnd, std::inserter(rComparator, rComparator.begin()));

	// We know I'm not empty, so rhs must be a shorter version of me and therefore I have precedence.
	if (rComparator.empty())
		return true;

	// Our set differences are not empty, must have unique values and are already sorted, so only need to compare the first element
	return *lComparator.begin() < *rComparator.begin();
}

bool mappers::DynamicIdeasRule::operator==(const DynamicIdeasRule& rhs) const
{
	if (replacee != rhs.replacee)
		return false;
	if (replacementIdentifier != rhs.replacementIdentifier)
		return false;
	if (precedenceLevel != rhs.precedenceLevel)
		return false;
	if (newEffect != rhs.newEffect)
		return false;

	std::vector<mappers::RulePair> lhsRuleSorted = ruleInfo;
	std::sort(lhsRuleSorted.begin(), lhsRuleSorted.end());
	std::vector<mappers::RulePair> rhsRuleSorted = rhs.ruleInfo;
	std::sort(rhsRuleSorted.begin(), rhsRuleSorted.end());

	return lhsRuleSorted == rhsRuleSorted;
}

bool mappers::DynamicIdeasRule::testRule(const RULE_TYPE ruleType, const std::string& ruleValue, const std::shared_ptr<CK3::Culture> culture) const
{
	switch (ruleType)
	{
		case RULE_TYPE::HERITAGE:
			return culture->getHeritage() == ruleValue;
		case RULE_TYPE::ETHOS_TRIGGER:
			return culture->getEthos() == ruleValue;
			// Possible extensions of the rules
			/*case RULE_TYPE::COASTAL:
				return coastalRule(ruleValue, culture) // For more complex rules, pass sourceWorld down chain? Use friend fxns?
			case RULE_TYPE::REGION:
				return regionRule(ruleValue, culture) // Future complexity could complicate scopes. Not sure how to deal with.
			case RULE_TYPE::RELIGION:
				return religionRule(ruleValue, culture)*/
	}
	return false;
}

std::ostream& mappers::operator<<(std::ostream& output, const DynamicIdeasRule& rule)
{
	output << rule.getReplacement() + " with {";
	for (auto& pred: rule.getPrecedence())
		output << " " + std::to_string(pred);
	output << " } Precedence\n";

	return output;
}