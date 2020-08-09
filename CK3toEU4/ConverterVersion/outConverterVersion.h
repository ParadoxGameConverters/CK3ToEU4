#ifndef OUT_CONVERTER_VERSION
#define OUT_CONVERTER_VERSION



#include "ConverterVersion.h"
#include <ostream>



namespace ConverterVersion
{

std::ostream& operator<<(std::ostream& output, const ConverterVersion& version);

}



#endif // OUT_CONVERTER_VERSION