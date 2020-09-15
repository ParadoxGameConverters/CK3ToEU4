#ifndef EU4_AREA_H
#define EU4_AREA_H

#include "Parser.h"
#include <map>
#include <memory>

namespace EU4
{
class Province;
}
namespace mappers
{
class Area: commonItems::parser
{
  public:
	explicit Area(std::istream& theStream);

	[[nodiscard]] const auto& getProvinces() const { return provinces; }
	[[nodiscard]] bool areaContainsProvince(int province) const;

	void linkProvince(const std::pair<int, std::shared_ptr<EU4::Province>>& theProvince) { provinces[theProvince.first] = theProvince.second; }

  private:
	void registerKeys();
	std::map<int, std::shared_ptr<EU4::Province>> provinces;
};
} // namespace mappers

#endif // EU4_AREA_H