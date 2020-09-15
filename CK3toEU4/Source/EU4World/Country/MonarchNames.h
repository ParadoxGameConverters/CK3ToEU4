#ifndef EU4_MONARCH_NAMES_H
#define EU4_MONARCH_NAMES_H
#include <istream>
#include <map>

namespace EU4
{

class MonarchNames
{
  public:
	MonarchNames() = default;
	explicit MonarchNames(std::istream& theStream);

	[[nodiscard]] const auto& getMonarchNames() const { return monarchNames; }

  private:
	std::map<std::string, std::pair<int, int>> monarchNames; // name (without regnal), pair(regnal, chance)
};
} // namespace EU4

#endif // EU4_MONARCH_NAMES_H
