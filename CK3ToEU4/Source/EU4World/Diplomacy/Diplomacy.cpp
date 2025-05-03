#include "Diplomacy.h"
#include "../../CK3World/Titles/Title.h"
#include "../Country/Country.h"
#include "CK3ToEU4/Source/CK3World/Characters/Character.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

EU4::Diplomacy::Diplomacy()
{
	Log(LogLevel::Info) << "-> Loading Eastern Diplomacy";
	registerKeys();
	parseFile("configurables/chinese_tributaries.txt");
	clearRegisteredKeywords();
	Log(LogLevel::Info) << ">> " << agreements.size() << " tributaries loaded.";
}

void EU4::Diplomacy::registerKeys()
{
	registerKeyword("dependency", [this](const std::string& type, std::istream& theStream) {
		auto newAgreement = std::make_shared<Agreement>(theStream, type);
		agreements.emplace_back(newAgreement);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void EU4::Diplomacy::importVassals(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	// Vassalages are our own creation so we're pinging our countries alone.
	for (const auto& country: countries)
	{
		if (!country.second->getTitle())
			continue;
		for (const auto& targetTitle: country.second->getTitle()->second->getGeneratedVassals())
		{
			const auto& targetTag = targetTitle.second->getEU4Tag();
			if (!targetTag || targetTag->second->getProvinces().empty())
				continue;

			auto newAgreement = std::make_shared<Agreement>(country.first, targetTag->first, "vassal", country.second->getConversionDate());
			agreements.push_back(newAgreement);
		}
	}
}

void EU4::Diplomacy::generateTributaries(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	// Tributaries are our own creation so we're pinging our countries alone.
	for (const auto& country: countries)
	{
		if (!country.second->getTitle())
			continue;
		const auto& holder = country.second->getTitle()->second->getHolder();
		if (!holder)
			continue; // No holder? Jeez.
		const auto& suzerain = holder->second->getSuzerain();
		if (!suzerain)
			continue; // No tributary overlord. We're good, carry on.
		const auto& suzCharDomain = suzerain->second->getCharacterDomain();
		if (!suzCharDomain)
			continue; // Suzerain without domain, how...(?!)
		const auto& suzDomain = suzCharDomain->getDomain();
		if (suzDomain.empty())
			continue; // what.
		const auto& suzPrimaryTitle = suzDomain.begin();
		if (!suzPrimaryTitle->second)
			continue;
		const auto& targetTag = suzPrimaryTitle->second->getEU4Tag();
		if (!targetTag)
			continue;
		if (targetTag->second->getProvinces().empty())
			continue;

		auto newAgreement = std::make_shared<Agreement>(targetTag->first, country.first, "dependency", "tributary_state", country.second->getConversionDate());
		agreements.push_back(newAgreement);
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

void EU4::Diplomacy::filterDeadRelationships(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	Log(LogLevel::Info) << "-> Filtering dead relationships.";

	std::vector<std::shared_ptr<Agreement>> newAgreements;
	std::set<std::string> landlessCountries;

	for (const auto& [countryTag, country]: countries)
		if (country->getProvinces().empty())
			landlessCountries.insert(countryTag);

	// All countries we process must have provinces.
	for (const auto& agreement: agreements)
		if (landlessCountries.contains(agreement->getFirst()) && agreement->getFirst() != "MNG" || landlessCountries.contains(agreement->getSecond()))
			continue;
		else
			newAgreements.emplace_back(agreement);

	Log(LogLevel::Info) << "<> " << agreements.size() - newAgreements.size() << " dead relationship(s) filtered.";

	agreements.swap(newAgreements);
}
