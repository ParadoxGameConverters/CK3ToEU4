#include "Diplomacy.h"
#include "../../CK2World/Titles/Title.h"
#include "../Country/Country.h"
#include "Log.h"
#include "ParserHelpers.h"

EU4::Diplomacy::Diplomacy()
{
	LOG(LogLevel::Info) << "-> Loading Eastern Diplomacy";
	registerKeys();
	parseFile("configurables/chinese_tributaries.txt");
	clearRegisteredKeywords();
	LOG(LogLevel::Info) << ">> " << agreements.size() << " tributaries loaded.";
}

void EU4::Diplomacy::registerKeys()
{
	registerKeyword("dependency", [this](const std::string& type, std::istream& theStream) {
		auto newAgreement = std::make_shared<Agreement>(theStream, type);
		agreements.emplace_back(newAgreement);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void EU4::Diplomacy::importAgreements(const std::map<std::string, std::shared_ptr<Country>>& countries, const CK2::Diplomacy& diplomacy, date conversionDate)
{
	LOG(LogLevel::Info) << "-> Explaining Diplomacy Like It's Five";
	importVassals(countries);
	importTributaries(countries, diplomacy, conversionDate);
}

void EU4::Diplomacy::importVassals(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	// Vassalages are our own creation os we're pinging our countries alone.
	for (const auto& country: countries)
	{
		if (country.second->getTitle().first.empty())
			continue;
		for (const auto& targetTitle: country.second->getTitle().second->getGeneratedVassals())
		{
			const auto& targetTag = targetTitle.second->getEU4Tag();
			if (targetTag.first.empty())
				continue;
			if (targetTag.second->getProvinces().empty())
				continue;

			auto newAgreement = std::make_shared<Agreement>(country.first, targetTag.first, "vassal", country.second->getConversionDate());
			agreements.push_back(newAgreement);
		}
	}
}

void EU4::Diplomacy::importTributaries(const std::map<std::string, std::shared_ptr<Country>>& countries, const CK2::Diplomacy& diplomacy, date conversionDate)
{

	// Tributaries are a personal matter and we need to map it to country holders.
	// First make reference maps of all relevant tags, their holders and tributaries.

	std::map<int, std::string> holderTags;						// holderID, Tag
	std::map<int, std::map<int, bool>> holderTributaries; // holder ID, tributaries/vassals (not necessarily holders that survived).

	for (const auto& country: countries)
	{
		if (country.second->getTitle().first.empty())
			continue;
		if (!country.second->getTitle().second->getHolder().first)
			continue;
		// Let's skip personal unions and vassals.
		if (isCountryVassal(country.first) || isCountryJunior(country.first))
			continue;

		const auto& holder = country.second->getTitle().second->getHolder();
		holderTags.insert(std::pair(holder.first, country.first));

		const auto& allRelations = diplomacy.getDiplomacy();
		const auto& relationsItr = allRelations.find(holder.first);
		if (relationsItr == allRelations.end())
			continue; // No relations.
		const auto& holderRelations = relationsItr->second;
		for (const auto& relation: holderRelations.getRelations())
		{
			if (relation.isFirstOverlord()) // CK2 tributaries are messy. Dual pairings are not guaranteed.
			{
				holderTributaries[holder.first].insert(std::pair(relation.getSecond(), relation.getTributaryType() == "autonomous_tributary"));
			}
			else if (relation.isSecondOverlord())
			{
				holderTributaries[relation.getSecond()].insert(std::pair(relation.getFirst(), relation.getTributaryType() == "autonomous_tributary"));
			}
		}
	}

	// Loading is complete, now let's match people with countries.

	for (const auto& holderItr: holderTributaries)
	{
		const auto& first = holderTags[holderItr.first]; // overlord
		for (const auto& subHolderItr: holderItr.second)
		{
			// Does this fellow have a country?
			if (!holderTags.count(subHolderItr.first))
				continue;
			// He does! Excellent!
			const auto& second = holderTags[subHolderItr.first];

			// Make a tributary/vassal agreement
			if (subHolderItr.second)
			{
				auto newAgreement = std::make_shared<Agreement>(first, second, "vassal", conversionDate);
				agreements.push_back(newAgreement);
			}
			else
			{
				auto newAgreement = std::make_shared<Agreement>(first, second, "dependency", "tributary_state", conversionDate);
				agreements.push_back(newAgreement);
			}
		}
	}
}

void EU4::Diplomacy::updateTagsInAgreements(const std::string& oldTag, const std::string& newTag)
{
	for (const auto& agreement: agreements)
		agreement->updateTags(oldTag, newTag);
}

void EU4::Diplomacy::deleteAgreementsWithTag(const std::string& deadTag)
{
	std::vector<std::shared_ptr<Agreement>> newAgreements;
	for (const auto& agreement: agreements)
	{
		if (agreement->getFirst() == deadTag || agreement->getSecond() == deadTag)
			continue;
		newAgreements.emplace_back(agreement);
	}
	agreements.swap(newAgreements);
}

bool EU4::Diplomacy::isCountryVassal(const std::string& tag) const
{
	for (const auto& agreement: agreements)
	{
		if (agreement->getSecond() == tag && agreement->getType() == "vassal")
			return true;
	}
	return false;
}

bool EU4::Diplomacy::isCountryJunior(const std::string& tag) const
{
	for (const auto& agreement: agreements)
	{
		if (agreement->getSecond() == tag && agreement->getType() == "union")
			return true;
	}
	return false;
}
