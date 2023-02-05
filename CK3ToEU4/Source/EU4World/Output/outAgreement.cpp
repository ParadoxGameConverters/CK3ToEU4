#include "outAgreement.h"

std::ostream& EU4::operator<<(std::ostream& output, const Agreement& agreement)
{
	output << agreement.type << "=\n";
	output << "{\n";
	if (!agreement.subjectType.empty())
		output << "\tsubject_type=\"" << agreement.subjectType << "\"\n";
	output << "\tfirst=\"" << agreement.first << "\"\n";
	output << "\tsecond=\"" << agreement.second << "\"\n";
	output << "\tstart_date=\"" << agreement.startDate << "\"\n";
	output << "\tend_date=\"1836.1.1\"\n";
	output << "}\n";
	output << "\n";
	return output;
}
