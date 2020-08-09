#include "outConverterVersion.h"



std::ostream& ConverterVersion::operator<<(std::ostream& output, const ConverterVersion& version)
{
	output << "Converter version " << version.version << " \"" << version.name << "\" - "
		<< version.descriptionLine;
	return output;
}