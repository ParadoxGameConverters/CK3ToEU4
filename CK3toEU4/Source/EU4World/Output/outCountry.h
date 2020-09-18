#ifndef OUT_COUNTRY_H
#define OUT_COUNTRY_H

#include "../Country/Country.h"
#include "../Country/CountryDetails.h"
#include <ostream>

namespace EU4
{
std::ostream& operator<<(std::ostream& output, const Country& country);
} // namespace EU4
#endif // OUT_COUNTRY_H