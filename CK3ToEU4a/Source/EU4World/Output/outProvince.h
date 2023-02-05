#ifndef OUT_PROVINCE_H
#define OUT_PROVINCE_H

#include "../Province/EU4Province.h"

namespace EU4
{
std::ostream& operator<<(std::ostream& output, const Province& province);
} // namespace EU4


#endif // OUT_PROVINCE_H