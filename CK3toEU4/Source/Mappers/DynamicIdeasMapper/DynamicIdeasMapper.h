#ifndef DYNAMIC_IDEAS_MAPPER
#define DYNAMIC_IDEAS_MAPPER

#include "DynamicIdeasLink.h"
#include "Parser.h"

namespace mappers
{
class DynamicIdeasMapper: commonItems::parser
{
  public:
	DynamicIdeasMapper();

	[[nodiscard]] const auto& getEthosMap() const { return ethosMap; }
	[[nodiscard]] const auto& getTraditionMap() const { return traditionMap; }
	[[nodiscard]] const auto& getDefaults() const { return defaults; }

  private:
	void registerKeys();

	std::map<std::string, std::vector<AssignmentPair>> ethosMap;
	std::map<std::string, std::vector<AssignmentPair>> traditionMap;
	std::vector<std::string> defaults; // Used to fill in cultures with less than 8 traditions
};
} // namespace mappers
#endif // DYNAMIC_IDEAS_MAPPER