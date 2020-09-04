#ifndef EU4_WORLD_H
#define EU4_WORLD_H
#include "../CK3World/World.h"
#include "../Mappers/ConverterVersion/ConverterVersion.h"
#include "Output/outModFile.h"

class Configuration;

namespace CK3
{
} // namespace CK3

namespace EU4
{
class World
{
  public:
	World(const CK3::World& sourceWorld, const Configuration& theConfiguration, const mappers::ConverterVersion& converterVersion);

  private:
	void output(const mappers::ConverterVersion& converterVersion, const Configuration& theConfiguration, const CK3::World& sourceWorld) const;
	void createModFile(const Configuration& theConfiguration) const;
	void outputVersion(const mappers::ConverterVersion& versionParser, const Configuration& theConfiguration) const;
	void outputInvasionExtras(const Configuration& theConfiguration) const;
	void outputBookmark(const Configuration& theConfiguration, date conversionDate) const;

	ModFile modFile;
};
} // namespace EU4

#endif // EU4_WORLD_H
