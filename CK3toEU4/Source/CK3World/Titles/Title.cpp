#include "Title.h"
#include "../../Mappers/DevWeightsMapper/DevWeightsMapper.h"
#include "../Characters/Character.h"
#include "../Geography/CountyDetail.h"
#include "../Geography/ProvinceHolding.h"
#include "LandedTitles.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Title::Title(std::istream& theStream, long long theID): ID(theID)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	// Obligatory sanity check.
	if (!holder)
	{
		// This title does NOT in fact exist.
		dfLiege.reset();
	}
}

void CK3::Title::registerKeys()
{
	registerKeyword("key", [this](const std::string& unused, std::istream& theStream) {
		name = commonItems::singleString(theStream).getString();
	});
	registerKeyword("name", [this](const std::string& unused, std::istream& theStream) {
		displayName = commonItems::singleString(theStream).getString();
		if (displayName.find("\x15") != std::string::npos)
		{
			cleanUpDisplayName();
		}
	});
	registerKeyword("adj", [this](const std::string& unused, std::istream& theStream) {
		adjective = commonItems::singleString(theStream).getString();
	});
	registerKeyword("date", [this](const std::string& unused, std::istream& theStream) {
		creationDate = date(commonItems::singleString(theStream).getString());
	});
	registerKeyword("claim", [this](const std::string& unused, std::istream& theStream) {
		for (auto claimantID: commonItems::llongList(theStream).getLlongs())
			claimants.insert(std::make_pair(claimantID, nullptr));
	});
	registerKeyword("history_government", [this](const std::string& unused, std::istream& theStream) {
		historyGovernment = commonItems::singleString(theStream).getString();
	});
	registerKeyword("theocratic_lease", [this](const std::string& unused, std::istream& theStream) {
		theocraticLease = commonItems::singleString(theStream).getString() == "yes";
	});
	registerKeyword("capital_barony", [this](const std::string& unused, std::istream& theStream) {
		cCapitalBarony = commonItems::singleString(theStream).getString() == "yes";
	});
	registerKeyword("duchy_capital_barony", [this](const std::string& unused, std::istream& theStream) {
		dCapitalBarony = commonItems::singleString(theStream).getString() == "yes";
	});
	registerKeyword("capital", [this](const std::string& unused, std::istream& theStream) {
		capital = std::pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
	});
	registerKeyword("de_facto_liege", [this](const std::string& unused, std::istream& theStream) {
		dfLiege = std::pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
	});
	registerKeyword("de_jure_liege", [this](const std::string& unused, std::istream& theStream) {
		djLiege = std::pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
	});
	registerKeyword("de_jure_vassals", [this](const std::string& unused, std::istream& theStream) {
		for (auto vassalID: commonItems::llongList(theStream).getLlongs())
			djVassals.insert(std::make_pair(vassalID, nullptr));
	});
	registerKeyword("heir", [this](const std::string& unused, std::istream& theStream) {
		for (auto heirID: commonItems::llongList(theStream).getLlongs())
			heirs.emplace_back(std::make_pair(heirID, nullptr));
	});
	registerKeyword("laws", [this](const std::string& unused, std::istream& theStream) {
		const auto& theLaws = commonItems::stringList(theStream).getStrings();
		laws = std::set(theLaws.begin(), theLaws.end());
	});
	registerKeyword("holder", [this](const std::string& unused, std::istream& theStream) {
		holder = std::pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
	});
	registerKeyword("coat_of_arms_id", [this](const std::string& unused, std::istream& theStream) {
		coa = std::pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
	});
	registerKeyword("succession_election", [this](const std::string& unused, std::istream& theStream) {
		const auto newTitle = Title(theStream, 0);
		electors = newTitle.getElectors();
	});
	registerKeyword("electors", [this](const std::string& unused, std::istream& theStream) {
		for (auto electorID: commonItems::intList(theStream).getInts())
			electors.insert(std::pair(electorID, nullptr));
	});
	registerKeyword("landless", [this](const std::string& unused, std::istream& theStream) {
		landless = commonItems::singleString(theStream).getString() == "yes";
	});
	registerKeyword("color", [this](const std::string& unused, std::istream& theStream) {
		color = laFabricaDeColor.getColor(theStream);
	});
	registerKeyword("history", [this](const std::string& unused, std::istream& theStream) {
		previousHolders = Title(theStream, 0).getPreviousHolders();
	});
	registerRegex(R"(\d+.\d+.\d+)", [this](const std::string& unused, std::istream& theStream) {
		const auto questionableItem = commonItems::stringOfItem(theStream).getString();
		auto tempStream = std::stringstream(questionableItem);
		if (questionableItem.find('{') == std::string::npos)
		{
			try
			{
				previousHolders.emplace_back(std::pair(commonItems::singleLlong(tempStream).getLlong(), nullptr));
			}
			catch (std::exception&)
			{
				Log(LogLevel::Warning) << "Invalid previous holder ID: " << questionableItem;
			}
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void CK3::Title::cleanUpDisplayName()
{
	auto start = displayName.find('\x15');
	auto silence = false;
	auto silenceOverRide = false;
	auto braceDepth = 0;
	std::string newName;
	while (start != std::string::npos)
	{
		if ((!silence || silenceOverRide) && start != 0)
		{
			newName += displayName.substr(0, start);
			displayName = displayName.substr(start, displayName.length());
			silence = true;
			silenceOverRide = false;
			start = displayName.find('\x15');
		}
		else if (displayName.at(start + 1) == 'L')
		{
			if (displayName.length() >= start + 3)
				displayName = displayName.substr(start + 3, displayName.length());
			else
				break;
			silence = true;
			silenceOverRide = true;
			braceDepth++;
			start = displayName.find('\x15');
		}
		else if (displayName.at(start + 1) == '!')
		{
			if (displayName.length() >= start + 2)
				displayName = displayName.substr(start + 2, displayName.length());
			else
				break;
			braceDepth--;
			if (braceDepth <= 0)
			{
				silence = false;
				braceDepth = 0;
			}
			silenceOverRide = false;
			start = displayName.find('\x15');
		}
		else
		{
			silence = true;
			silenceOverRide = false;
			if (displayName.length() >= start + 1)
				displayName = displayName.substr(start + 1, displayName.length());
			else
				break;
			braceDepth++;
			start = displayName.find('\x15');
		}
	}
	displayName = newName;
}


int CK3::Title::flagDeJureHREProvinces()
{
	auto counter = 0;
	// For CK3 flagging dejure alone is not enough as we'll be losing a ton of land when duchies break away in full independence.
	// Flag our dejure AND defacto counties, or our dejure/defacto vassals' counties, wherever those may be.
	// Actual flagging will happen on county level as higher titles have no dejure provinces linked to them at this stage.
	// We're leaving baronies out of this, counties are sufficient.
	for (const auto& deJureVassal: djVassals)
	{
		counter += deJureVassal.second->flagDeJureHREProvinces();
	}
	for (const auto& deFactoVassal: dfVassals)
	{
		counter += deFactoVassal.second->flagDeJureHREProvinces();
	}
	if (!clay) // We really need the county clay to confirm we're a landful proper county.
		return counter;
	if (!clay->getCounty() || getLevel() != LEVEL::COUNTY)
		return counter;

	clay->getCounty()->second->setDeJureHRE();
	++counter;

	return counter;
}

void CK3::Title::brickTitle()
{
	grantIndependence();

	// Drop from own holder's domain.
	if (holder)
		holder->second->dropTitleFromDomain(ID);
	// Drop holder
	holder.reset();

	// release all vassals
	for (const auto& vassal: dfVassals)
		vassal.second->grantIndependence();
	dfVassals.clear(); // just in case?
}

void CK3::Title::dropTitleFromDFVassals(long long titleID)
{
	const auto& dfvItr = dfVassals.find(titleID);
	if (dfvItr != dfVassals.end())
		dfVassals.erase(dfvItr);
}

void CK3::Title::grantIndependence()
{
	// Drop this title from liege holder's vassals
	if (dfLiege)
		dfLiege->second->dropTitleFromDFVassals(ID);

	// Drop liege
	dfLiege.reset();
}

std::map<std::string, std::shared_ptr<CK3::Title>> CK3::Title::coalesceDFCounties() const
{
	// We're gathering vassal's counties + self (if c_), and passing them on, adding nothing to ourselves.
	std::map<std::string, std::shared_ptr<Title>> toReturn;
	for (const auto& vassal: dfVassals)
	{
		const auto& vassalCounties = vassal.second->coalesceDFCounties();
		toReturn.insert(vassalCounties.begin(), vassalCounties.end());
	}
	toReturn.insert(ownedDFCounties.begin(), ownedDFCounties.end());
	return toReturn;
}

std::map<std::string, std::shared_ptr<CK3::Title>> CK3::Title::coalesceDJCounties() const
{
	// We're gathering vassal dejure provinces + our own, and passing them on, adding nothing to ourselves.
	std::map<std::string, std::shared_ptr<Title>> toReturn;
	for (const auto& deJureVassal: djVassals)
	{
		const auto& vassalDJCounties = deJureVassal.second->coalesceDJCounties();
		toReturn.insert(vassalDJCounties.begin(), vassalDJCounties.end());
	}
	toReturn.insert(ownedDJCounties.begin(), ownedDJCounties.end());
	return toReturn;
}

void CK3::Title::congregateDFCounties()
{
	// We're gathering vassal counties and adding to our own.
	for (const auto& vassal: dfVassals)
	{
		const auto& vassalDFCounties = vassal.second->coalesceDFCounties();
		ownedDFCounties.insert(vassalDFCounties.begin(), vassalDFCounties.end());
	}
}

void CK3::Title::congregateDJCounties()
{
	// We're gathering de jure vassal de jure counties and adding to our own.
	for (const auto& deJureVassal: djVassals)
	{
		const auto& deJureVassalDJCounties = deJureVassal.second->coalesceDJCounties();
		ownedDJCounties.insert(deJureVassalDJCounties.begin(), deJureVassalDJCounties.end());
	}
}

CK3::LEVEL CK3::Title::getLevel() const
{
	// It's easy, until it's not.
	if (name.find("b_") == 0)
		return LEVEL::BARONY;
	if (name.find("c_") == 0)
		return LEVEL::COUNTY;
	if (name.find("d_") == 0)
		return LEVEL::DUCHY;
	if (name.find("k_") == 0)
		return LEVEL::KINGDOM;
	if (name.find("e_") == 0)
		return LEVEL::EMPIRE;

	// This is the questionable part. It should work for customs as they are treated identically as any other - with dejure parents and all.
	// exceptions are dynamic mercs, landless religious orders and similar - but those don't hold land, at least initially.

	// see if they hold any vassals and if so, assign a level one step higher.
	auto level = -1;
	const std::set<char> allowedPrefixes = {'b', 'c', 'd', 'k'};
	for (const auto& vassal: djVassals) // run through all as they can vary in levels.
	{
		if (!allowedPrefixes.count(vassal.second->getName().at(0)))
			continue; // Those dynamic vassals can't help us. Or heaven's forbid, it's an empire.
		if (LevelToInt[vassal.second->getLevel()] > level)
			level = LevelToInt[vassal.second->getLevel()] + 1;
	}
	if (level > -1)
		return IntToLevel[std::max(level, 4)];

	// Without vassals we must poke at our hierarchy, if any.
	if (djLiege)
		return IntToLevel[LevelToInt[djLiege->second->getLevel()] - 1];
	else
		return LEVEL::EMPIRE; // If this is wrong for landless mercs, it won't affect anything as they are landless anyway.
}

std::optional<commonItems::Color> CK3::Title::getColor() const
{
	if (color)
		return color;
	if (clay && clay->getColor())
		return clay->getColor();
	return std::nullopt;
}

bool CK3::Title::isLandless() const
{
	if (landless)
		return true;
	if (clay && clay->isLandless())
		return true;
	return false;
}

double CK3::Title::getBuildingWeight(const mappers::DevWeightsMapper& devWeightsMapper) const
{
	if (getLevel() != LEVEL::COUNTY) // This applies to nothing but counties.
		return 0;

	// buildingWeight is a mixture of all holdings, their potential buildings, and general county development.
	if (!clay)
		throw std::runtime_error("County " + name + " has no clay?");
	if (!clay->getCounty() || !clay->getCounty()->second)
		throw std::runtime_error("County " + name + " has no county in its clay?");
	const auto development = clay->getCounty()->second->getDevelopment();
	auto buildingCount = 0;
	auto holdingCount = 0;

	for (const auto& barony: djVassals)
	{
		if (!barony.second)
			throw std::runtime_error("Running unlinked vassals, are we? " + std::to_string(barony.first) + " has no link.");
		if (!barony.second->getClay())
			throw std::runtime_error("Supposed barony " + barony.second->getName() + " of " + name + " has no clay?");
		if (!barony.second->getClay()->getProvince() || !barony.second->getClay()->getProvince()->second)
			throw std::runtime_error("Barony " + barony.second->getName() + " of " + name + " has no clay province?");
		const auto& baronyProvince = barony.second->getClay()->getProvince();
		buildingCount += baronyProvince->second->countBuildings();
		if (!baronyProvince->second->getHoldingType().empty())
			++holdingCount;
	}

	const auto totalDev = devWeightsMapper.getDevFromHolding() * holdingCount + devWeightsMapper.getDevFromBuilding() * buildingCount +
								 devWeightsMapper.getDevFromDev() * development;
	return totalDev;
}

void CK3::Title::relinkDeFactoVassals()
{
	// We're reconstructing defacto hierarchy; we're redirecting our DFvassals from our DFtitle into our DJtitle - if holder of this title holds hold both.
	// This is important - otherwise secondary titles wouldn't have their own vassals under them but under primary title, breaking PU splitoffs,
	// and resulting with those secondary duchies/kingdoms being discarded as landless.

	if (name.starts_with("c_") || name.starts_with("b_"))
		return; // don't bother with counties and below.

	for (const auto& dfVassal: dfVassals)
	{
		if (!dfVassal.second)
			continue;

		if (dfVassal.second->getDFLiege() && dfVassal.second->getDJLiege() && dfVassal.second->getDFLiege()->first != dfVassal.second->getDJLiege()->first)
		{
			// Do we own both titles?
			if (holder && holder->second && holder->second->getDomain())
			{
				const auto holderTitles = holder->second->getDomain()->getDomain();

				// Make a small cache of the holder's owned title names.
				std::set<std::string> titleNameCache;
				for (const auto& domainTitle: holderTitles)
					titleNameCache.insert(domainTitle.second->getName());

				if (titleNameCache.contains(dfVassal.second->getDFLiege()->second->getName()) &&
					 titleNameCache.contains(dfVassal.second->getDJLiege()->second->getName()))
				{
					// We do own both. Tell the vassal to relink its defacto owner to dejure title.
					dfVassal.second->loadDFLiege(*dfVassal.second->getDJLiege());
					// Tell dejure title it's now owner of a brand new vassal.
					dfVassal.second->getDJLiege()->second->addDFVassals(std::map{dfVassal});
					// And finally...
					dropTitleFromDFVassals(dfVassal.first);
				}
			}
			else
			{
				if (!holder)
					Log(LogLevel::Warning) << name << " has no holder but has vassals?";
				else if (!holder->second)
					Log(LogLevel::Warning) << name << "'s holder is not linked up!";
				else if (!holder->second->getDomain())
					Log(LogLevel::Warning) << name << "' holder has no domain but has vassals?";
			}
		}
	}
}
