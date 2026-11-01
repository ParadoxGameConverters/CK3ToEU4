#include "CK3ToEU4Converter.h"
#include "CK3World/World.h"
#include "Configuration/Configuration.h"
#include "Diagnostics/Diagnostics.h"
#include "EU4World/EU4World.h"
#include "Log.h"
#include <filesystem>
#include <optional>

void convertCK3ToEU4(const commonItems::ConverterVersion& converterVersion)
{
	Log(LogLevel::Progress) << "0 %";
	const std::filesystem::path reportPath = "debug/conversion_report.txt";
	try
	{
		const auto theConfiguration = std::make_shared<Configuration>(converterVersion);
		const CK3::World sourceWorld(theConfiguration, converterVersion);
		std::optional<EU4::World> destWorld;
		{
			diagnostics::ScopedTimer timer("EU4 world build");
			destWorld.emplace(sourceWorld, *theConfiguration, converterVersion);
		}
		Log(LogLevel::Notice) << "* Conversion complete *";
		Log(LogLevel::Progress) << "100 %";
	}
	catch (const std::exception& e)
	{
		// The report is most useful when the conversion fails, so write it before the error reaches main.
		diagnostics::Report::instance().recordIssue(diagnostics::Severity::Error, "fatal", e.what());
		diagnostics::Report::instance().write(reportPath);
		throw;
	}
	diagnostics::Report::instance().write(reportPath);
}
