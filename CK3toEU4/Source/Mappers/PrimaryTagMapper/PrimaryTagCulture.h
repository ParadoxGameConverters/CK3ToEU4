#ifndef PRIMARY_TAG_CULTURE_H
#define PRIMARY_TAG_CULTURE_H

#include "Parser.h"

namespace mappers
{
class PrimaryTagCulture: commonItems::parser
{
  public:
	PrimaryTagCulture() = default;
	explicit PrimaryTagCulture(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> getTag() const;

  private:
	void registerKeys();

	std::string tag;
};
} // namespace mappers

#endif // PRIMARY_TAG_CULTURE_H