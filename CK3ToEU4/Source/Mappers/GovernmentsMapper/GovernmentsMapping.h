#ifndef GOVERNMENTS_MAPPING
#define GOVERNMENTS_MAPPING

#include "Parser.h"
#include <set>

namespace mappers
{
class GovernmentsMapping: commonItems::parser
{
  public:
	GovernmentsMapping() = default;
	explicit GovernmentsMapping(std::istream& theStream);

	[[nodiscard]] const auto& getGovernment() const { return government; }
	[[nodiscard]] const auto& getCK3Governments() const { return ck3governments; }
	[[nodiscard]] const auto& getReform() const { return reform; }
	[[nodiscard]] const auto& getCK3Title() const { return ck3title; }

	[[nodiscard]] bool matchGovernment(const std::string& ck3Government, const std::string& incTitle) const;

  private:
	void registerKeys();

	std::string government;
	std::string reform;
	std::string ck3title;
	std::set<std::string> ck3governments;
};
} // namespace mappers

#endif // GOVERNMENTS_MAPPING