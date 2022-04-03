#ifndef DYNAMIC_IDEAS_LINK
#define DYNAMIC_IDEAS_LINK

#include "Parser.h"

namespace mappers
{

typedef struct AssignmentPair
{
	std::string type;
	std::string value;

	bool operator<(const AssignmentPair& rhs) const { return value < rhs.value; }
	bool operator==(const AssignmentPair& rhs) const { return type == rhs.type && value == rhs.value; }
} AssignmentPair;

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
	[[nodiscard]] const auto& getPair() const { return ambiguosPairs; }

  private:
	void registerKeys();

	std::optional<std::string> ethos;
	std::optional<std::string> tradition;
	std::optional<std::string> defaultString;
	std::optional<std::string> ideaName;
	std::vector<AssignmentPair> effects; // (type = "global_manpower_modifier", value = "0.10")
	std::vector<AssignmentPair> rules;	 // (type = "heritage", value = "heritage_north_germanic")

	// Turns into either rules or effects
	std::vector<AssignmentPair> ambiguousPairs; // assigned specifics based on calling scope
};
} // namespace mappers
#endif // DYNAMIC_IDEAS_LINK