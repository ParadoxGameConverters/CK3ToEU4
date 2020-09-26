#ifndef OUT_RELIGION_H
#define OUT_RELIGION_H

#include "../Religion/GeneratedReligion.h"
#include <ostream>

namespace EU4
{
std::ostream& operator<<(std::ostream& output, const GeneratedReligion& religion);
}

#endif // OUT_RELIGION_H