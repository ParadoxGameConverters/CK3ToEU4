#include "Log.h"
#include "VersionParser/VersionParser.h"
#include "CK3ToEU4.h"



int main()
{
	try
	{
		const mappers::VersionParser versionParser;
		Log(LogLevel::Info) << versionParser;
		Log(LogLevel::Info) << "Built on " << __TIMESTAMP__;
		Log(LogLevel::Progress) << "0%";
		convertCK3ToEU4();

		return 0;
	}

	catch (const std::exception& e)
	{
		Log(LogLevel::Error) << e.what();
		return -1;
	}
}