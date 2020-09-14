#include "Diplomacy.h"
#include "../../CK3World/Titles/Title.h"
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

void EU4::Diplomacy::importVassals(const std::map<std::string, std::shared_ptr<Country>>& countries)
{
	// Vassalages are our own creation os we're pinging our countries alone.
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
