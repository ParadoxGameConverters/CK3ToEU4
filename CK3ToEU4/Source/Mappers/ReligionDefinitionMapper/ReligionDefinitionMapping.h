#ifndef RELIGION_DEFINITION_MAPPING_H
#define RELIGION_DEFINITION_MAPPING_H

#include "Parser.h"

namespace mappers
{
class ReligionDefinitionMapping: commonItems::parser
{
  public:
	explicit ReligionDefinitionMapping(std::istream& theStream);

	[[nodiscard]] const auto& getCountry() const { return country; }
	[[nodiscard]] const auto& getProvince() const { return province; }
	[[nodiscard]] const auto& getCountrySecondary() const { return countrySecondary; }
	[[nodiscard]] const auto& getUnique() const { return unique; }
	[[nodiscard]] const auto& getNonUnique() const { return nonUnique; }
	[[nodiscard]] const auto& getAllowedConversion() const { return allowedConversion; }

  private:
	void registerKeys();

	std::string country;
	std::string province;
	std::string countrySecondary;
	std::string unique;
	std::string nonUnique;
	std::string allowedConversion;
};
} // namespace mappers

#endif // RELIGION_DEFINITION_MAPPING_H