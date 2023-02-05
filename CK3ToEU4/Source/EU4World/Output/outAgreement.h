#ifndef OUT_AGREEMENT_H
#define OUT_AGREEMENT_H

#include "../Diplomacy/Agreement.h"

namespace EU4
{
std::ostream& operator<<(std::ostream& output, const Agreement& agreement);
}

#endif // OUT_AGREEMENT_H