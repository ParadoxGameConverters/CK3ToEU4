#ifndef CK3_HOUSE_H
#define CK3_HOUSE_H
#include "Parser.h"

namespace CK3
{
class House: commonItems::parser
{
  public:
	House() = default;
	House(std::istream& theStream, int houseID);

  private:
	void registerKeys();

	int houseID = 0;
};
} // namespace CK3

#endif // CK3_HOUSE_H
