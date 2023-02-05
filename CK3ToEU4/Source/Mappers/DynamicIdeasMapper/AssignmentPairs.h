#ifndef ASSIGNMENT_PAIRS_H
#define ASSIGNMENT_PAIRS_H

#include "DynamicIdeasRuleEnum.h"
#include <string>

namespace mappers
{
typedef struct EffectPair
{
	std::string modifier;
	std::string value;

	bool operator==(const EffectPair& rhs) const { return modifier == rhs.modifier && value == rhs.value; }
} EffectPair;
typedef struct RulePair
{
	RULE_TYPE rule_type;
	std::string value;

	bool operator<(const RulePair& rhs) const { return value < rhs.value; }
	bool operator==(const RulePair& rhs) const { return rule_type == rhs.rule_type && value == rhs.value; }
} RulePair;
} // namespace mappers
#endif // ASSIGNMENT_PAIRS_H