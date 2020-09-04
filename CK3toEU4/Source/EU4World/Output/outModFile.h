#ifndef OUTPUT_H
#define OUTPUT_H

#include <ostream>

namespace EU4
{
class ModFile
{
  public:
	std::string outname;

	friend std::ostream& operator<<(std::ostream& output, const ModFile& modFile);
};
std::ostream& operator<<(std::ostream& output, const ModFile& modFile);
} // namespace EU4

#endif // OUTPUT_H