#ifndef CK3_FAITHS_H
#define CK3_FAITHS_H
#include "Parser.h"

namespace CK3
{
class Faith;
class Religions;
class Faiths: commonItems::parser
{
  public:
	Faiths() = default;
	explicit Faiths(std::istream& theStream);

	[[nodiscard]] const auto& getFaiths() const { return faiths; }

	void linkReligions(const Religions& religions);

  private:
	void registerKeys();

	std::map<long long, std::shared_ptr<Faith>> faiths;
};
} // namespace CK3

#endif // CK3_FAITHS_H
