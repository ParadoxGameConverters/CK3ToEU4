#include "outConverterVersion.h"

std::ostream& mappers::operator<<(std::ostream& output, const ConverterVersion& version)
{
	output << "\n\n";
	output << "************ -= The Paradox Converters Team =- ********************\n";
	output << "* Converter version " << version.version << " \"" << version.name << "\"\n";
	output << "* " << version.getDescription() << "\n";
	output << "* Built on " << __TIMESTAMP__ << "\n";
	output << "********************** + CK3 To EU4 + *****************************\n";
	return output;
}