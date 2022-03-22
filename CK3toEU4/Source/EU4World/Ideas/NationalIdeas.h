#ifndef EU4_NATIONAL_IDEAS_H
#define EU4_NATIONAL_IDEAS_H
#include "../../CK3World/Cultures/Culture.h"
#include "../../Mappers/DynamicIdeasMapper/DynamicIdeasMapper.h"
#include <string>
#include <vector>

namespace EU4
{
class NationalIdeas
{
  public:
	NationalIdeas() = default;
	NationalIdeas(std::shared_ptr<CK3::Culture> culture, const mappers::DynamicIdeasMapper& dynIdeasMapper);

	[[nodiscard]] const auto& getEthos() const { return culture->getEthos(); }
	[[nodiscard]] const auto& getDynamicName() const { return culture->getName(); }
	[[nodiscard]] const auto& getLocalizedName() const { return localizedName; }
	[[nodiscard]] const auto& getTraditionIdeas() const { return traditionIdeas; }
	[[nodiscard]] const auto& getEthosEffects() const { return ethosEffects; }
	[[nodiscard]] const auto& getTraditionEffects() const { return traditionEffects; }
	[[nodiscard]] const auto& getCulture() const { return culture; }

	friend std::ostream& operator<<(std::ostream& output, const NationalIdeas& country);

  private:
	std::string localizedName;					  // Defined version of culture's optional value if undefined there
	std::vector<std::string> traditionIdeas; // Becomes national ideas/ambitions, filled up to 8 if culture had less
	std::vector<mappers::AssignmentPair> ethosEffects;
	std::vector<std::vector<mappers::AssignmentPair>> traditionEffects;

	std::shared_ptr<CK3::Culture> culture;
};
} // namespace EU4

#endif // EU4_NATIONAL_IDEAS_H