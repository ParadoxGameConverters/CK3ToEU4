#include "DynamicIdeasRule.h"
#include "OSCompatibilityLayer.h"

mappers::DynamicIdeasRule::DynamicIdeasRule(const std::vector<AssignmentPair>& ruleInfo,
	 const std::vector<AssignmentPair>& newEffect,
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
		const auto& precedence = StringToRuleType.at(rule.type);
		precedenceLevel.emplace(RuleTypeToPrecedence.at(precedence));

		// Craft unique identfier name
		// Support coastal = yes and heritage = heritage_north_germanic while keeping as much brevity as possible
		if (!ideaName)
		{
			const std::string& prefix = getLeadStr(rule.type);
			if (rule.value.contains(prefix))
				replacementIdentifier += ("_" + rule.value);
			else
				replacementIdentifier += ("_" + rule.type + "_" + rule.value);
		}
	}
}

bool mappers::DynamicIdeasRule::testRule(const std::shared_ptr<CK3::Culture> culture) const
{
	// Generate rule
	for (const auto& rule: ruleInfo)
	{
		const auto& ruleType = StringToRuleType.at(rule.type);

		// Shortcut return false if any rule doesn't pass, otherwise return true
		switch (ruleType)
		{
			case RULE_TYPE::HERITAGE:
				if (culture->getHeritage() != rule.value)
					return false;
				break;
			case RULE_TYPE::ETHOS_TRIGGER:
				if (culture->getEthos() != rule.value)
					return false;
				break;
				// Possible extensions of the rules
				/*case RULE_TYPE::COASTAL:
					if (!coastalRule(rule.value, idea)) // For more complex rules, pass sourceWorld down chain? Use friend fxns?
						return false;
					break;
				case RULE_TYPE::REGION:
					if (!regionRule(rule.value, idea)) // Future complexity could complicate scopes. Not sure how to deal with.
						return false;
					break;
				case RULE_TYPE::RELIGION:
					if (!religionRule(rule.value, idea))
						return false;
					break;*/
		}
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

	// Acutal Strict weak ordering
	const auto& lhsStart = precedenceLevel.begin();
	const auto& lhsEnd = precedenceLevel.end();
	const auto& rhsStart = rhs.precedenceLevel.begin();
	const auto& rhsEnd = rhs.precedenceLevel.end();

	std::set<int> lComparator;
	std::set_difference(lhsStart, lhsEnd, rhsStart, rhsEnd, std::inserter(lComparator, lComparator.begin()));

	if (lComparator.empty())
		return false;

	std::set<int> rComparator;
	std::set_difference(rhsStart, rhsEnd, lhsStart, lhsEnd, std::inserter(rComparator, rComparator.begin()));

	if (rComparator.empty())
		return true;
	else
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

	std::vector<mappers::AssignmentPair> lhsRuleSorted = ruleInfo;
	std::sort(lhsRuleSorted.begin(), lhsRuleSorted.end());
	std::vector<mappers::AssignmentPair> rhsRuleSorted = rhs.ruleInfo;
	std::sort(rhsRuleSorted.begin(), rhsRuleSorted.end());

	return lhsRuleSorted == rhsRuleSorted;
}

std::ostream& mappers::operator<<(std::ostream& output, const DynamicIdeasRule& rule)
{
	output << rule.getReplacement() + " with {";
	for (auto& pred: rule.getPrecedence())
		output << " " + std::to_string(pred);
	output << " } Precedence\n";

	return output;
}