#ifndef DYNAMIC_IDEAS_RULE_ENUM_H
#define DYNAMIC_IDEAS_RULE_ENUM_H

#include <map>
#include <string>
namespace mappers
{
enum class RULE_TYPE
{
	HERITAGE,
	ETHOS_TRIGGER
};

static std::map<RULE_TYPE, int> RuleTypeToPrecedence{{RULE_TYPE::HERITAGE, 0}, {RULE_TYPE::ETHOS_TRIGGER, 1}};
static std::map<std::string, RULE_TYPE> StringToRuleType{{"heritage", RULE_TYPE::HERITAGE}, {"ethos_rule", RULE_TYPE::ETHOS_TRIGGER}};
static std::map<RULE_TYPE, std::string> RuleTypeToString{{RULE_TYPE::HERITAGE, "heritage"}, {RULE_TYPE::ETHOS_TRIGGER, "ethos_rule"}};
} // namespace mappers
#endif // DYNAMIC_IDEAS_RULE_ENUM_H