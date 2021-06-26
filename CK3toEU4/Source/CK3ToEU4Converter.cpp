#include "CK3ToEU4Converter.h"
#include "Configuration/Configuration.h"
#include "CK3World/World.h"
#include "EU4World/EU4World.h"
#include "Log.h"

void convertCK3ToEU4(const mappers::ConverterVersion& converterVersion)
{
	Log(LogLevel::Progress) << "0 %";
	const auto theConfiguration = std::make_shared<Configuration>(converterVersion);
	const CK3::World sourceWorld(theConfiguration, converterVersion);
	EU4::World destWorld(sourceWorld, *theConfiguration, converterVersion);
	LOG(LogLevel::Info) << "* Conversion complete *";
	Log(LogLevel::Progress) << "100 %";
}
