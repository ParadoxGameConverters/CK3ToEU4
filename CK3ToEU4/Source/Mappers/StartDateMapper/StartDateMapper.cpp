#include "StartDateMapper.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::StartDateMapper::StartDateMapper()
{
	registerKeys();
	parseFile(std::filesystem::path("configurables/start_date.txt"));
	clearRegisteredKeywords();
	validateDate();
}

mappers::StartDateMapper::StartDateMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
	validateDate();
}

void mappers::StartDateMapper::registerKeys()
{
	registerKeyword("year", [this](const std::string& unused, std::istream& theStream) {
		year = commonItems::getInt(theStream);
	});
	registerKeyword("month", [this](const std::string& unused, std::istream& theStream) {
		month = commonItems::getInt(theStream);
	});
	registerKeyword("day", [this](const std::string& unused, std::istream& theStream) {
		day = commonItems::getInt(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void mappers::StartDateMapper::validateDate()
{
	// No leap years in PDX games.
	std::set<int> twentyEightMonths = {2};
	std::set<int> thirtyMonths = {4, 6, 9, 11};

	if (year < 500 || year > 1821 || month < 1 || month > 12 || day < 1 || day > 31)
	{
		fallbackToDefault();
		return;
	}

	if (twentyEightMonths.contains(month) && day > 28)
	{
		fallbackToDefault();
		return;
	}

	if (thirtyMonths.contains(month) && day > 30)
	{
		fallbackToDefault();
		return;
	}

	startDate = date(year, month, day);
}

void mappers::StartDateMapper::fallbackToDefault()
{
	Log(LogLevel::Error) << "Invalid start date set in start_date.txt: " << year << "/" << month << "/" << day << " - falling back to 1444/11/11.";
	startDate = date(1444, 11, 11);
}
