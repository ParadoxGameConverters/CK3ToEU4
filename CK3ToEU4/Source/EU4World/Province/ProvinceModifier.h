#ifndef EU4_PROVINCE_MODIFIER_H
#define EU4_PROVINCE_MODIFIER_H

#include "Parser.h"

namespace EU4
{
class ProvinceModifier: commonItems::parser
{
  public:
	ProvinceModifier() = default;
	explicit ProvinceModifier(std::istream& theStream);

	// These values are open to ease management.
	// This is a storage container for EU4::ProvinceDetails.
	std::string name;
	int duration = -1;

  private:
	void registerKeys();
};
} // namespace EU4

#endif // EU4_PROVINCE_MODIFIER_H
