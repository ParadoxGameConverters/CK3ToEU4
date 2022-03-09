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
	NationalIdeas(std::vector<std::string> tagsVector, CK3::Culture& culture, mappers::DynamicIdeasMapper& dynIdeasMapper);

	[[nodiscard]] const auto& getEthos() const { return ethos; }
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getTags() const { return tags; }
	[[nodiscard]] const std::vector<std::string> getTraditions() const { return traditions; }
	[[nodiscard]] const auto& getMapper() const { return dynIdeasMapper; }
	[[nodiscard]] const auto& getLocalizedName() const { return localizedName; }

	friend std::ostream& operator<<(std::ostream& output, const NationalIdeas& country);

  private:

	std::string ethos;							// Becomes national traditions
	std::string name;
	std::string localizedName;					// For localizing idea/tradition/ambition
	std::vector<std::string> traditions;		// Becomes national ideas/ambitions
	std::vector<std::string> tags;				// All tags that should have this idea group

	const mappers::DynamicIdeasMapper& dynIdeasMapper;
	
};
} // namespace EU4

#endif // EU4_NATIONAL_IDEAS_H