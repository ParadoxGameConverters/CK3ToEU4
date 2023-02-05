#ifndef OUT_CHARACTER_H
#define OUT_CHARACTER_H

#include "../Country/Country.h"
#include "../Country/CountryDetails.h"

namespace EU4
{
std::ostream& operator<<(std::ostream& output, const Character& character);
} // namespace EU4
#endif // OUT_CHARACTER_H