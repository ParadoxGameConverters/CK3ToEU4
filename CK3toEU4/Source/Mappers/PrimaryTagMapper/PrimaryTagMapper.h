#ifndef PRIMARY_TAG_MAPPER_H
#define PRIMARY_TAG_MAPPER_H

#include "../../Configuration/Configuration.h"
#include "Parser.h"

namespace mappers
{
class PrimaryTagMapper: commonItems::parser
{
  public:
	PrimaryTagMapper() = default;
	explicit PrimaryTagMapper(std::istream& theStream);

	void loadPrimaryTags(const Configuration& theConfiguration);
	[[nodiscard]] std::optional<std::string> getPrimaryTagForCulture(const std::string& culture) const;

	[[nodiscard]] const auto& getCultureTags() const { return cultureTags; } // for testing

  private:
	void registerKeys();

	std::map<std::string, std::string> cultureTags;
};
} // namespace mappers

#endif // PRIMARY_TAG_MAPPER_H