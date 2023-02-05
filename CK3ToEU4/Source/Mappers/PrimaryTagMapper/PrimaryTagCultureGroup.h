#ifndef PRIMARY_TAG_CULTURE_GROUP_H
#define PRIMARY_TAG_CULTURE_GROUP_H

#include "Parser.h"

namespace mappers
{
class PrimaryTagCultureGroup: commonItems::parser
{
  public:
	PrimaryTagCultureGroup() = default;
	explicit PrimaryTagCultureGroup(std::istream& theStream);

	[[nodiscard]] const auto& getCollectedTags() const { return cultureTags; }

  private:
	void registerKeys();

	std::map<std::string, std::string> cultureTags;
};
} // namespace mappers

#endif // PRIMARY_TAG_CULTURE_GROUP_H