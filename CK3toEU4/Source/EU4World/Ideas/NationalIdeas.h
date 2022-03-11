#ifndef EU4_NATIONAL_IDEAS_H
#define EU4_NATIONAL_IDEAS_H
#include "../../CK3World/Cultures/Culture.h"
#include "../../Mappers/DynamicIdeasMapper/DynamicIdeasMapper.h"
#include <vector>
#include <string>

namespace EU4
{
class NationalIdeas
{
  public:
	NationalIdeas() = default;
	NationalIdeas(std::shared_ptr<CK3::Culture> culture, mappers::DynamicIdeasMapper& dynIdeasMapper);

	[[nodiscard]] const auto& getEthos() const { return culture->getEthos(); }
	[[nodiscard]] const auto& getDynamicName() const { return culture->getName(); }
	[[nodiscard]] const auto& getLocalizedName() const { return localizedName; }
	[[nodiscard]] const std::vector<std::string> getTraditionIdeas() const { return traditionIdeas; }
	[[nodiscard]] const auto& getMapper() const { return dynIdeasMapper; }

	friend std::ostream& operator<<(std::ostream& output, const NationalIdeas& country);

  private:
	std::string localizedName;						// Defined version of culture's optional value if undefined there
	std::vector<std::string> traditionIdeas;		// Becomes national ideas/ambitions, filled up to 8 if culture had less

	std::shared_ptr<CK3::Culture> culture;
	const mappers::DynamicIdeasMapper& dynIdeasMapper; // Parser for config file
	
};
} // namespace EU4

#endif // EU4_NATIONAL_IDEAS_H