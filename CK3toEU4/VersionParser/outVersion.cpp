#include "outVersion.h"



std::ostream& mappers::operator<<(std::ostream& output, const VersionParser& versionParser)
{
	output << "Converter version " << versionParser.version << " \"" << versionParser.name << "\" - "
			 << versionParser.descriptionLine;
	return output;
}