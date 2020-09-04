#ifndef CK3_DYNASTIES_H
#define CK3_DYNASTIES_H
#include "Parser.h"

namespace CK3
{
class Dynasty;
class Dynasties: commonItems::parser
{
  public:
	Dynasties() = default;
	explicit Dynasties(std::istream& theStream); // For testing

	[[nodiscard]] const auto& getDynasties() const { return dynasties; }

  private:
	void registerKeys();

	std::map<int, std::shared_ptr<Dynasty>> dynasties;
};
} // namespace CK3

#endif // CK3_DYNASTIES_H
