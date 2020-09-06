#ifndef CK3_FAITHS_H
#define CK3_FAITHS_H
#include "Parser.h"

namespace CK3
{
class Faith;
class Faiths: commonItems::parser
{
  public:
	Faiths() = default;
	explicit Faiths(std::istream& theStream); // For testing

	[[nodiscard]] const auto& getFaiths() const { return faiths; }

  private:
	void registerKeys();

	std::map<int, std::shared_ptr<Faith>> faiths;
};
} // namespace CK3

#endif // CK3_FAITHS_H
