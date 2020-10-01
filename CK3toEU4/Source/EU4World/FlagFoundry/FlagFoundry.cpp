#include "FlagFoundry.h"
#include "../../CK3World/CoatsOfArms/CoatOfArms.h"
#include "../../CK3World/Dynasties/House.h"
#include "../../CK3World/Titles/Title.h"
#include "../../Configuration/Configuration.h"
#include "../Country/Country.h"
#include "Magick++.h"
#include "OSCompatibilityLayer.h"
#include "Warehouse.h"
#include "../Religion/GeneratedReligion.h"

// This is the only class that interacts with imageMagick, outside of EU4World, which depends on this one.

EU4::FlagFoundry::FlagFoundry()
{
	Magick::InitializeMagick(".");
	warehouse = std::make_shared<Warehouse>();
	flagCrafter.loadWarehouse(warehouse);
}

void EU4::FlagFoundry::loadImageFolder(const Configuration& theConfiguration) const
{
	warehouse->loadImageFolder(theConfiguration.getCK3Path() + "gfx/coat_of_arms/");
}

void EU4::FlagFoundry::generateFlags(const std::map<std::string, std::shared_ptr<Country>>& countries, const Configuration& theConfiguration) const
{
	// prep the battleground.
	if (!commonItems::DeleteFolder("flags.tmp"))
		Log(LogLevel::Error) << "Could not delete flags.tmp folder!";
	else if (!commonItems::TryCreateFolder("flags.tmp"))
		throw std::runtime_error("Could not create flags.tmp folder!");

	// For every tag we must have a flag. This can be vanilla flag (in which case we do nothing), or
	// our pre-generated flag (which we copy over) or a dynamic flag from CK3 savegame which we have yet to create.

	// Hardcoding special flags from out-of-scope we're not supposed to craft.
	const std::set<std::string> ignoreTags = {"SDM"};

	for (const auto& country: countries)
	{
		if (ignoreTags.count(country.first))
			continue;
		
		// first check is for dynasty and override flags.
		if (country.second->getHasDynastyName()) // If this is true, we have a holder, house and dynasty already.
		{
			const auto dynastyID = country.second->getHouse()->getDynasty().first;
			// Do we have an appropriate flag?
			if (commonItems::DoesFileExist("configurables/dynastyflags/" + std::to_string(dynastyID) + ".tga"))
			{
				commonItems::TryCopyFile("configurables/dynastyflags/" + std::to_string(dynastyID) + ".tga", "flags.tmp/" + country.first + ".tga");
				continue; // and this country is done.
			}
		}

		// Do we have an alternate flag source?
		if (commonItems::DoesFileExist("blankMod/output/gfx/flags/" + country.first + ".tga"))
			continue; // This will be copied over by outWorld.

		// Otherwise, do we need a flag at all?
		if (commonItems::DoesFileExist(theConfiguration.getEU4Path() + "/gfx/flags/" + country.first + ".tga"))
			continue; // We'll be using vanilla flag.

		// We do.
		craftFlag(country.second);
	}

	// Do not forget about our SDM.
	if (theConfiguration.getSunset() == Configuration::SUNSET::ACTIVE)
		commonItems::TryCopyFile("configurables/sunset/gfx/flags/SDM.tga", "flags.tmp/SDM.tga");
}

void EU4::FlagFoundry::craftFlag(const std::shared_ptr<Country>& country) const
{
	if (!country->getTitle())
	{
		Log(LogLevel::Error) << "Cannot craft a flag for country " << country->getTag() << " - it has no Title!";
		return;
	}
	if (!country->getTitle()->second->getCoA())
	{
		Log(LogLevel::Error) << "Cannot craft a flag for country " << country->getTag() << " - title " << country->getTitle()->first << " has no CoA!";
		return;
	}

	const auto& coa = country->getTitle()->second->getCoA()->second;
	auto generatedCoa = flagCrafter.craftFlagFromCoA(*coa);
	try
	{
		generatedCoa.magick("TGA");
		generatedCoa.write("flags.tmp/" + country->getTag() + ".tga");
	}
	catch (std::exception& e)
	{
		Log(LogLevel::Error) << "Failed exporting flag: " << e.what();
	}
}

void EU4::FlagFoundry::extendReligionStrips(const Configuration& theConfiguration, const std::vector<GeneratedReligion>& religions) const
{
	std::set<std::string> targetStrips = {"country_icon_religion.dds", "icon_religion.dds", "icon_religion_small.dds", "province_view_religion.dds"};
	for (const auto& religion: religions)
	{
		// Import the source dds
		Magick::Image sourceIcon;
		if (!religion.iconPath.empty())
		{
			const auto path1 = theConfiguration.getCK3Path() + religion.iconPath; // one of these two should be it.
			const auto path2 = theConfiguration.getCK3Path() + "/gfx/interface/icons/religion/" + religion.iconPath + ".dds";
			if (commonItems::DoesFileExist(path1))
				sourceIcon.read(path1);
			else if (commonItems::DoesFileExist(path2))
				sourceIcon.read(path2);
			else
			{
				Log(LogLevel::Warning) << "Could not find religious icon: " << religion.iconPath << ", skipping!";				
				sourceIcon = Magick::Image("100x100", Magick::Color("transparent")); // blank.
			}
		}
		else
		{
			Log(LogLevel::Warning) << "Religion " << religion.name << " has no icon set, skipping!";
			sourceIcon = Magick::Image("100x100", Magick::Color("transparent")); // blank.
		}
		// Process target strips
		for (const auto& target: targetStrips)
		{
			if (!commonItems::DoesFileExist("output/" + theConfiguration.getOutputName() + "/gfx/interface/" + target))
				throw std::runtime_error("output/" + theConfiguration.getOutputName() + "/gfx/interface/" + target + "! Where are our religion strips?");
			Magick::Image targetStrip("output/" + theConfiguration.getOutputName() + "/gfx/interface/" + target);
			targetStrip = extendReligionStrip(targetStrip, sourceIcon);
			targetStrip.write("output/" + theConfiguration.getOutputName() + "/gfx/interface/" + target);
		}
	}
}

Magick::Image EU4::FlagFoundry::extendReligionStrip(const Magick::Image& sourceStrip, const Magick::Image& icon) const
{
	const auto width = sourceStrip.size().width();
	const auto height = sourceStrip.size().height();
	const auto dimension = height;
	const auto newWidth = width + dimension;

	// Make a new image.
	auto newStrip = Magick::Image(Magick::Geometry(newWidth, height), Magick::Color("transparent"));
	// Paste old strip
	newStrip.composite(sourceStrip, "0x0", MagickCore::OverCompositeOp);
	// Resize icon
	auto newIcon = icon;
	newIcon.adaptiveResize(Magick::Geometry(dimension, dimension));
	// paste new icon in the back
	newStrip.composite(newIcon, width, 0, MagickCore::OverCompositeOp);
	// and done.
	return newStrip;
}