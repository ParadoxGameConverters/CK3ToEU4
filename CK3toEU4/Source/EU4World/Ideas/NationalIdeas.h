#ifndef EU4_NATIONAL_IDEAS_H
#define EU4_NATIONAL_IDEAS_H
#include "../../CK3World/Cultures/Culture.h"
#include "../../Mappers/DynamicIdeasMapper/DynamicIdeasMapper.h"
#include <vector>
#include <string>

namespace EU4
{
class NationalIdeas // Class wasn't nessecary, but might make it easier for future tweaks
{
  public:
	NationalIdeas() = default;
	NationalIdeas(std::shared_ptr<CK3::Culture> culture, mappers::DynamicIdeasMapper& dynIdeasMapper);

	[[nodiscard]] const auto& getEthos() const { return ethos; }
	[[nodiscard]] const auto& getCulturalName() const { return culturalName; }
	[[nodiscard]] const std::vector<std::string> getTraditions() const { return traditions; }
	[[nodiscard]] const auto& getMapper() const { return dynIdeasMapper; }
	[[nodiscard]] const auto& getLocalizedName() const { return localizedName; }

	friend std::ostream& operator<<(std::ostream& output, const NationalIdeas& country);

  private:

	std::string ethos;							// Becomes national traditions
	std::string culturalName;					// Culture identifier e.g. dynamic-irish-polish-culture-num3
	std::string localizedName;					// For localizing idea/tradition/ambition
	std::vector<std::string> traditions;		// Becomes national ideas/ambitions

	const mappers::DynamicIdeasMapper& dynIdeasMapper; // Parser for config file
	
};
} // namespace EU4

#endif // EU4_NATIONAL_IDEAS_H