#ifndef OUTPUT_CULTURE_DEFINITIONS_H
#define OUTPUT_CULTURE_DEFINITIONS_H
#include "../../Mappers/CultureDefinitionsMapper/CultureDefinitionsMapper.h"
#include "../../Mappers/CultureDefinitionsMapper/CultureDefiniton.h"
#include "../../Mappers/CultureDefinitionsMapper/CultureGroupDefinition.h"

namespace mappers
{
std::ostream& operator<<(std::ostream& output, const CultureDefinition& cultureDefinition);
std::ostream& operator<<(std::ostream& output, const CultureGroupDefinition& cultureGroupDefinition);
std::ostream& operator<<(std::ostream& output, const CultureDefinitionsMapper& cultureDefinitionsMapper);
} // namespace mappers

#endif // OUTPUT_CULTURE_DEFINITIONS_H