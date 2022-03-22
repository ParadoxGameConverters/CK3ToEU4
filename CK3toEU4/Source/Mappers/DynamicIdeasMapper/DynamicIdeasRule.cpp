#include "DynamicIdeasRule.h"
#include "OSCompatibilityLayer.h"

mappers::DynamicIdeasRule::DynamicIdeasRule(std::vector<AssignmentPair> ruleInfo, std::string replacee, std::vector<AssignmentPair> newEffect):
	 ruleInfo(ruleInfo), replacee(replacee), newEffect(newEffect), replacementIdentifier(replacee)
{
	for (const auto& rule: ruleInfo)
	{
		// Set precedance level
		const auto& precedence = StringToRuleType.at(rule.type);
		precedenceLevel.emplace(RuleTypeToPrecedence.at(precedence));

		// Craft unique identfier name
		// Support coastal = yes and heritage = heritage_north_germanic while keeping as much brevity as possible
		const std::string& prefix = getLeadStr(rule.type);
		if (rule.value.contains(prefix))
			replacementIdentifier += ("_" + rule.value);
		else
			replacementIdentifier += ("_" + rule.type + "_" + rule.value);
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
	if (this->precedenceLevel == rhs.precedenceLevel)
	{
		if (this->ruleInfo == rhs.ruleInfo)
			return this->replacee != rhs.replacee;
		else
			return true;
	}

	// Acutal Strict weak ordering
	const auto& lhsStart = this->precedenceLevel.begin();
	const auto& lhsEnd = this->precedenceLevel.end();
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
	bool tracker = replacee == rhs.replacee && replacementIdentifier == rhs.replacementIdentifier && precedenceLevel == rhs.precedenceLevel;

	if (!tracker)
		return tracker;

	tracker = tracker && newEffect == rhs.newEffect;

	std::vector<mappers::AssignmentPair> lhsRuleSorted = ruleInfo;
	std::sort(lhsRuleSorted.begin(), lhsRuleSorted.end());
	std::vector<mappers::AssignmentPair> rhsRuleSorted = rhs.ruleInfo;
	std::sort(rhsRuleSorted.begin(), rhsRuleSorted.end());

	return tracker && lhsRuleSorted == rhsRuleSorted;
}

std::ostream& mappers::operator<<(std::ostream& output, const DynamicIdeasRule& rule)
{
	output << rule.getReplacement() + " with {";
	for (auto& pred: rule.getPrecedence())
		output << " " + std::to_string(pred);
	output << " } Precedence\n";

	return output;
}

std::string getLeadStr(const std::string& str)
{
	if (const auto& i = str.find('_'); i != std::string::npos)
		return str.substr(0, i);
	else
		return str;
}