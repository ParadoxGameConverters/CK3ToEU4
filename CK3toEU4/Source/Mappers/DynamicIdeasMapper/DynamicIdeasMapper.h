#ifndef DYNAMIC_IDEAS_MAPPER
#define DYNAMIC_IDEAS_MAPPER

#include "Parser.h"

namespace mappers
{
class DynamicIdeasMapper: commonItems::parser
{
  public:
	DynamicIdeasMapper();
	explicit DynamicIdeasMapper(std::istream& theStream);

	[[nodiscard]] const auto& getEthosMap() const { return ethosMap; }
	[[nodiscard]] const auto& getTraditionMap() const { return traditionMap; }
	[[nodiscard]] const auto& getDefaultMap() const { return defaultMap; }
																	  
  private:															  
	void registerKeys();											  

	std::map<std::string, std::vector<std::string>> ethosMap;
	std::map<std::string, std::vector<std::string>> traditionMap;
	std::map<std::string, std::vector<std::string>> defaultMap; // Used to fill in cultures with less than 8 traditions

};
} // namespace mappers
#endif // DYNAMIC_IDEAS_MAPPER