#include "MonarchNames.h"
#include "ParserHelpers.h"
#include "Log.h"
#include "StringUtils.h"

EU4::MonarchNames::MonarchNames(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::MonarchNames::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& nameBlock, std::istream& theStream) {
		auto chance = commonItems::singleInt(theStream).getInt();
		const auto nameData = parseName(nameBlock);
		if (nameData)
			monarchNames.insert(std::pair(nameData->first, std::pair(nameData->second, chance)));		
	});
}

std::optional<std::pair<std::string, int>> EU4::MonarchNames::parseName(const std::string& nameBlock)
{
	auto theBlock = nameBlock;
	if (theBlock.size() > 2 && theBlock[0] == '\"' && theBlock[theBlock.size() - 1] == '\"')
		theBlock = commonItems::remQuotes(theBlock);
	if (theBlock.find('#') != std::string::npos)
	{
		const auto hashpos = theBlock.find('#');
		const auto name = theBlock.substr(0, hashpos - 1);
		try
		{
			const auto regnal = std::stoi(theBlock.substr(hashpos + 1, theBlock.length()));
			return std::pair(name, regnal);
		}
		catch (std::exception& e)
		{
			Log(LogLevel::Warning) << "Broken Monarch regnal in name: " << theBlock << " : " << e.what();
			return std::nullopt;
		}
	}
	else
	{
		return std::pair(theBlock, 0);
	}
}
