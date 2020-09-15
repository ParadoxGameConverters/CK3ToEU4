#ifndef EU4_REGION_H
#define EU4_REGION_H

#include "Parser.h"

namespace mappers
{
class Area;
class Region: commonItems::parser
{
  public:
	explicit Region(std::istream& theStream);

	[[nodiscard]] const auto& getAreas() const { return areas; }
	[[nodiscard]] bool regionContainsProvince(int province) const;

	void linkArea(const std::pair<std::string, std::shared_ptr<Area>>& theArea) { areas[theArea.first] = theArea.second; }

  private:
	void registerKeys();
	std::map<std::string, std::shared_ptr<Area>> areas;
};
} // namespace mappers

#endif // EU4_REGION_H