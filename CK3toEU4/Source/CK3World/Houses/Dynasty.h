#ifndef CK3_DYNASTY_H
#define CK3_DYNASTY_H
#include "Parser.h"

namespace CK3
{
class Dynasty: commonItems::parser
{
  public:
	Dynasty() = default;
	Dynasty(std::istream& theStream, int theDynID);

	[[nodiscard]] auto getID() const { return dynID; }

  private:
	void registerKeys();

	int dynID = 0;
};
} // namespace CK3

#endif // CK3_DYNASTY_H
