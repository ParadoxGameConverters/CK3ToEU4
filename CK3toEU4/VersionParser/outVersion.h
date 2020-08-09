#ifndef OUT_VERSION
#define OUT_VERSION



#include "VersionParser.h"
#include <ostream>



namespace mappers
{

std::ostream& operator<<(std::ostream& output, const VersionParser& versionParser);

}



#endif // OUT_VERSION