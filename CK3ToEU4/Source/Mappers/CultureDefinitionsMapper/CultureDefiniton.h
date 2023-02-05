#ifndef CULTURE_DEFINITION_H
#define CULTURE_DEFINITION_H
#include "Parser.h"

namespace CK3
{
class Culture;
}

namespace mappers
{
class CultureDefinition: commonItems::parser
{
  public:
	CultureDefinition() = default;
	explicit CultureDefinition(std::istream& theStream);

	[[nodiscard]] const auto& getMaleNames() const { return maleNames; }
	[[nodiscard]] const auto& getFemaleNames() const { return femaleNames; }
	[[nodiscard]] const auto& getDynastyNames() const { return dynastyNames; }
	[[nodiscard]] const auto& getSourceCulture() const { return sourceCulture; }

	void addMaleNames(const std::vector<std::string>& names) { maleNames.insert(maleNames.end(), names.begin(), names.end()); }
	void addFemaleNames(const std::vector<std::string>& names) { femaleNames.insert(femaleNames.end(), names.begin(), names.end()); }
	void addDynastyNames(const std::vector<std::string>& names) { dynastyNames.insert(dynastyNames.end(), names.begin(), names.end()); }

	void setSourceCulture(const std::shared_ptr<CK3::Culture>& theCulture) { sourceCulture = theCulture; }
	friend std::ostream& operator<<(std::ostream& output, const CultureDefinition& cultureDefinition);

  private:
	void registerKeys();

	std::vector<std::string> maleNames;
	std::vector<std::string> femaleNames;
	std::vector<std::string> dynastyNames;
	std::shared_ptr<CK3::Culture> sourceCulture; // only relevant for dynamic cultures.
};
} // namespace mappers

#endif // CULTURE_DEFINITION_H
