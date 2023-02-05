#ifndef CK3_DYNAMICTEMPLATE_H
#define CK3_DYNAMICTEMPLATE_H
#include "Parser.h"

namespace CK3
{
class DynamicTemplate: commonItems::parser
{
  public:
	explicit DynamicTemplate(std::istream& theStream);

	[[nodiscard]] const auto& getDynamicTitleKey() const { return dynamicKey; }
	[[nodiscard]] const auto& getDynamicTitleRank() const { return dynamicRank; }

  private:
	void registerKeys();

	std::string dynamicKey;
	std::string dynamicRank;
};
} // namespace CK3

#endif // CK3_DYNAMICTEMPLATE_H
