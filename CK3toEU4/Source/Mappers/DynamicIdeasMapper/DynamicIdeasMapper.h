#ifndef DYNAMIC_IDEAS_MAPPER
#define DYNAMIC_IDEAS_MAPPER

#include "DynamicIdeasLink.h"
#include "DynamicIdeasRule.h"
#include "Parser.h"

namespace mappers
{

class DynamicIdeasMapper: commonItems::parser
{
  public:
	DynamicIdeasMapper() = default;
	DynamicIdeasMapper(const LocalizationMapper& localizationMapper);
	explicit DynamicIdeasMapper(std::istream& theStream, const LocalizationMapper& localizationMapper);	// for testing
	explicit DynamicIdeasMapper(std::string theStreamFile, const LocalizationMapper& localizationMapper); // for testing

	[[nodiscard]] const auto& getEthosMap() const { return ethosMap; }
	[[nodiscard]] const auto& getTraditionMap() const { return traditionMap; }
	[[nodiscard]] const auto& getDefaults() const { return defaults; }
	[[nodiscard]] const auto& getRules() const { return rules; }
	[[nodiscard]] const auto& getTraditionLocs() const { return traditionLocs; }

  private:
	void registerKeys();
	void processRules();

	std::map<std::string, std::vector<AssignmentPair>> ethosMap;
	std::map<std::string, std::vector<AssignmentPair>> traditionMap;
	std::vector<std::string> defaults; // Used to fill in cultures with less than 8 traditions
	std::set<DynamicIdeasRule> rules;
	std::map<std::string, LocBlock> traditionLocs;

	LocalizationMapper locs;
};
} // namespace mappers
#endif // DYNAMIC_IDEAS_MAPPER