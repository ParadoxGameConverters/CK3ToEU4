#ifndef CK3_COATOFARMS_H
#define CK3_COATOFARMS_H
#include "Parser.h"

namespace CK3
{
class CoatOfArms: commonItems::parser
{
  public:
	CoatOfArms() = default;
	CoatOfArms(std::istream& theStream, int ID);

  private:
	void registerKeys();

	int ID = 0;
};
} // namespace CK3

#endif // CK3_COATOFARMS_H
