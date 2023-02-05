#ifndef CK3_FLAGS_H
#define CK3_FLAGS_H
#include <set>

#include "Parser.h"

namespace CK3
{
class Flags: commonItems::parser
{
  public:
	Flags() = default;
	explicit Flags(std::istream& theStream);

	[[nodiscard]] const auto& getFlags() const { return flags; }

  private:
	void registerKeys();

	std::string itemType;
	std::string incomingFlag;
	std::set<std::string> flags;
};
} // namespace CK3

#endif // CK3_FLAGS_H
