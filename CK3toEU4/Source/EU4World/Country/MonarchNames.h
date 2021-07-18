#ifndef EU4_MONARCH_NAMES_H
#define EU4_MONARCH_NAMES_H
#include "Parser.h"
#include <map>

namespace EU4
{

class MonarchNames: commonItems::parser
{
  public:
	MonarchNames() = default;
	explicit MonarchNames(std::istream& theStream);

	[[nodiscard]] const auto& getMonarchNames() const { return monarchNames; }

  private:
	void registerKeys();
	static std::optional<std::pair<std::string, int>> parseName(const std::string& nameBlock);

	std::map<std::string, std::pair<int, int>> monarchNames; // name (without regnal), pair(regnal, chance)
};
} // namespace EU4

#endif // EU4_MONARCH_NAMES_H
