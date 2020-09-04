#ifndef OUT_CONVERTER_VERSION
#define OUT_CONVERTER_VERSION

#include "../../Mappers/ConverterVersion/ConverterVersion.h"
#include <ostream>

namespace mappers
{
std::ostream& operator<<(std::ostream& output, const ConverterVersion& version);
}



#endif // OUT_CONVERTER_VERSION