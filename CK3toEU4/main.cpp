#include "Log.h"
#include "ConverterVersion/ConverterVersion.h"
#include "CK3ToEU4.h"



int main()
{
	try
	{
		const ConverterVersion::ConverterVersion version;
		Log(LogLevel::Info) << version;
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