#include "Confederations.h"
#include "../Characters/Character.h"
#include "../Characters/Characters.h"
#include "../CoatsOfArms/CoatsOfArms.h"
#include "CommonRegexes.h"
#include "Confederation.h"
#include "ParserHelpers.h"
#include "ranges"

CK3::Confederations::Confederations(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Confederations::registerKeys()
{
	databaseParser.registerRegex(R"(\d+)", [this](const std::string& confederationID, std::istream& theStream) {
		const auto questionableItem = commonItems::stringOfItem(theStream).getString();
		auto tempStream = std::stringstream(questionableItem);
		if (questionableItem.find('{') != std::string::npos)
		{
			try
			{
				auto newConfederation = std::make_shared<Confederation>(tempStream, std::stoll(confederationID));
				confederations.insert(std::pair(newConfederation->getID(), newConfederation));
			}
			catch (std::exception&)
			{
				Log(LogLevel::Warning) << "Invalid confederation blob: " << questionableItem << " at ID: " << confederationID;
			}
		}
	});
	databaseParser.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	registerKeyword("database", [this](std::istream& theStream) {
		databaseParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void CK3::Confederations::linkCharacters(const Characters& characters)
{
	auto memberCounter = 0;

	const auto& characterData = characters.getCharacters();
	for (const auto& confederation: confederations | std::views::values)
	{
		std::map<long long, std::shared_ptr<Character>> members;

		for (const auto& memberID: confederation->getMembers() | std::views::keys)
		{

			const auto& characterDataItr = characterData.find(memberID);
			if (characterDataItr != characterData.end())
			{
				members.emplace(memberID, characterDataItr->second);
				++memberCounter;
			}
		}
		confederation->loadMembers(members);
	}

	Log(LogLevel::Info) << "<> " << memberCounter << " confederation members updated.";
}

void CK3::Confederations::linkCoats(const CoatsOfArms& coats)
{
	auto counter = 0;
	const auto& coatData = coats.getCoats();
	for (const auto& confederation: confederations)
	{
		if (!confederation.second->getCoat())
			continue;
		const auto& coatDataItr = coatData.find(confederation.second->getCoat()->first);
		if (coatDataItr != coatData.end())
		{
			confederation.second->loadCoat(*coatDataItr);
			++counter;
		}
		else
		{
			Log(LogLevel::Warning) << "Confederation " << confederation.second->getName() << " has CoA " << std::to_string(confederation.second->getCoat()->first)
										  << " which has no definition. Defaulting to leading member CoA.";
		}
	}
	Log(LogLevel::Info) << "<> " << counter << " confederations updated.";
}
