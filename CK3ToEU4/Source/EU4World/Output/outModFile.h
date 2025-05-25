#ifndef OUTPUT_H
#define OUTPUT_H

#include "GameVersion.h"

namespace EU4
{
class ModFile
{
  public:
	std::filesystem::path outname;
	GameVersion version;

	friend std::ostream& operator<<(std::ostream& output, const ModFile& modFile);
};
std::ostream& operator<<(std::ostream& output, const ModFile& modFile);
} // namespace EU4

#endif // OUTPUT_H