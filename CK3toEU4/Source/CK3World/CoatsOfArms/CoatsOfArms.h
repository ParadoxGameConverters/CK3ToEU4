#ifndef CK3_COATSOFARMS_H
#define CK3_COATSOFARMS_H
#include "Parser.h"

namespace CK3
{
class CoatOfArms;
class Titles;
class CoatsOfArms: commonItems::parser
{
  public:
	CoatsOfArms() = default;
	explicit CoatsOfArms(std::istream& theStream);
	[[nodiscard]] auto getCoats() const { return coats; }

	void linkParents(const Titles& titles);

  private:
	void registerKeys();

	std::map<long long, std::shared_ptr<CoatOfArms>> coats;
};
} // namespace CK3

#endif // CK3_COATSOFARMS_H
