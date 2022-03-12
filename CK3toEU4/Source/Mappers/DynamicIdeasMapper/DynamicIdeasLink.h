#ifndef DYNAMIC_IDEAS_LINK
#define DYNAMIC_IDEAS_LINK

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
	[[nodiscard]] const auto& getRules() const { return rules; }
	[[nodiscard]] const auto& getPair() const { return ambiguosPairs; }

																	  
  private:															  
	void registerKeys();											  

	std::optional<std::string> ethos;
	std::optional<std::string> tradition;
	std::optional<std::string> defaultString;
	std::vector<std::pair<std::string,std::string>> effects;
	std::vector<std::pair<std::string, std::string>> rules;

	// Turns into either rules or effects
	std::vector<std::pair<std::string, std::string>> ambiguosPairs;
};
} // namespace mappers
#endif // DYNAMIC_IDEAS_LINK