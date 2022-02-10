#ifndef CULTURE_GROUP_DEFINITION_H
#define CULTURE_GROUP_DEFINITION_H
#include "Parser.h"
#include <map>

namespace mappers
{
class CultureDefinition;
class CultureGroupDefinition: commonItems::parser
{
  public:
	CultureGroupDefinition() = default;
	CultureGroupDefinition(std::string theName, std::istream& theStream);

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getCultures() const { return cultures; }
	[[nodiscard]] const auto& getCulture(const std::string& cultureName) const { return cultures.at(cultureName); }
	[[nodiscard]] auto containsCulture(const std::string& cultureName) const { return cultures.contains(cultureName); }

	void mergeCulture(const std::string& theName, const std::shared_ptr<CultureDefinition>& culture);
	void setName(const std::string& theName) { name = theName; }

	friend std::ostream& operator<<(std::ostream& output, const CultureGroupDefinition& cultureGroupDefinition);

  private:
	void registerKeys();

	std::string name;
	std::map<std::string, std::shared_ptr<CultureDefinition>> cultures;
};
} // namespace mappers

#endif // CULTURE_GROUP_DEFINITION_H
