#include "FlagFoundry.h"
#include "../../CK3World/CoatsOfArms/CoatOfArms.h"
#include "../../CK3World/Dynasties/House.h"
#include "../../CK3World/Titles/Title.h"
#include "../../Configuration/Configuration.h"
#include "../Country/Country.h"
#include "../Religion/GeneratedReligion.h"
#include "CommonFunctions.h"
#include "Magick++.h"
#include "OSCompatibilityLayer.h"
#include "Warehouse.h"
#include <ranges>

// This is the only class that interacts with imageMagick, outside of EU4World, which depends on this one.

EU4::FlagFoundry::FlagFoundry()
{
	Magick::InitializeMagick(".");
	warehouse = std::make_shared<Warehouse>();
	flagCrafter.loadWarehouse(warehouse);
}

void EU4::FlagFoundry::loadImageFolders(const Configuration& theConfiguration, const Mods& mods) const
{
	std::vector<std::filesystem::path> folders;
	for (const auto& mod: mods | std::views::reverse)
	{
		if (!commonItems::DoesFolderExist(mod.path / "gfx/coat_of_arms"))
			continue;
		Log(LogLevel::Info) << "<> Loading some garments from [" << mod.name << "]";
		folders.push_back(mod.path / "gfx/coat_of_arms");
	}
	folders.push_back(theConfiguration.getCK3Path() / "gfx/coat_of_arms");
	warehouse->loadImageFolders(folders);
}

void EU4::FlagFoundry::generateFlags(const std::map<std::string, std::shared_ptr<Country>>& countries,
	 const Configuration& theConfiguration,
	 const std::vector<GeneratedReligion>& religions,
	 const Mods& mods) const
{
	// prep the battleground.
	if (std::filesystem::exists("flags.tmp") && std::filesystem::remove_all("flags.tmp") == static_cast<std::uintmax_t>(-1))
		Log(LogLevel::Error) << "Could not delete flags.tmp folder!";
	else if (!std::filesystem::create_directory("flags.tmp"))
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
			if (commonItems::DoesFileExist(std::filesystem::path("configurables/dynastyflags") / (std::to_string(dynastyID) + ".tga")))
			{
				std::filesystem::copy_file(std::filesystem::path("configurables/dynastyflags") / (std::to_string(dynastyID) + ".tga"),
					 std::filesystem::path("flags.tmp") / (country.first + ".tga"),
					 std::filesystem::copy_options::overwrite_existing);
				continue; // and this country is done.
			}
		}

		// Do we have an alternate flag source?
		if (commonItems::DoesFileExist(std::filesystem::path("blankMod/output/gfx/flags") / (country.first + ".tga")))
			continue; // This will be copied over by outWorld.

		if (theConfiguration.getPlayerTitle() && country.second->getTitle() && country.second->getTitle()->first == *theConfiguration.getPlayerTitle())
		{
			// We're crafting player's title flag manually.
			craftFlag(country.second);
			continue;
		}

		// Otherwise, do we need a flag at all?
		if (commonItems::DoesFileExist(theConfiguration.getEU4Path() / "gfx/flags" / (country.first + ".tga")))
			continue; // We'll be using vanilla flag.

		// We do.
		craftFlag(country.second);
	}

	// Do not forget about our SDM.
	if (theConfiguration.getSunset() == Configuration::SUNSET::ACTIVE)
		std::filesystem::copy_file("configurables/sunset/gfx/flags/SDM.tga", "flags.tmp/SDM.tga", std::filesystem::copy_options::overwrite_existing);

	// Time for Religious Rebels
	for (const auto& religion: religions)
	{
		craftRebelFlag(theConfiguration, religion, mods);
	}
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

	std::shared_ptr<CK3::CoatOfArms> coa;

	// Before we proceed, do we have a custom CoA to use?
	if (country->getConfederationCoA() && country->getConfederationCoA()->second)
	{
		coa = country->getConfederationCoA()->second;
	}
	else
	{
		coa = country->getTitle()->second->getCoA()->second;
	}

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

void EU4::FlagFoundry::craftRebelFlag(const Configuration& theConfiguration, const GeneratedReligion& religion, const Mods& mods) const
{
	// Import the generic Rebel Flag
	if (!commonItems::DoesFileExist(std::filesystem::path("blankMod/output/gfx/flags/generic_rebels.tga")))
		throw std::runtime_error("blankMod/output/gfx/flags/generic_rebels.tga! Where are the rebel scum!?");
	Magick::Image baseFlag("blankMod/output/gfx/flags/generic_rebels.tga");

	// Process target icon
	Magick::Image targetIcon;
	if (!religion.iconPath.empty())
	{
		auto foundIcon = false;
		auto path1 = theConfiguration.getCK3Path() / religion.iconPath; // one of these two should be it.
		auto iconPathDds = religion.iconPath;
		iconPathDds += ".dds";
		auto path2 = theConfiguration.getCK3Path() / "gfx/interface/icons/faith" / iconPathDds;
		if (commonItems::DoesFileExist(path1))
		{
			foundIcon = true;
			targetIcon.read(path1.string());
		}
		if (!foundIcon && commonItems::DoesFileExist(path2))
		{
			foundIcon = true;
			targetIcon.read(path2.string());
		}
		if (!foundIcon)
		{
			for (const auto& mod: mods)
			{
				path1 = mod.path / religion.iconPath;
				path2 = mod.path / "gfx/interface/icons/faith" / iconPathDds;
				if (commonItems::DoesFileExist(path1))
				{
					foundIcon = true;
					targetIcon.read(path1.string());
					break;
				}
				if (commonItems::DoesFileExist(path2))
				{
					foundIcon = true;
					targetIcon.read(path2.string());
					break;
				}
			}
		}
		if (!foundIcon)
		{
			Log(LogLevel::Warning) << "Could not find religious icon: " << religion.iconPath.string() << ", skipping!";
			targetIcon = Magick::Image("100x100", Magick::Color("transparent")); // blank.
		}
	}
	else
	{
		Log(LogLevel::Warning) << "Religion " << religion.name << " has no icon set, skipping!";
		targetIcon = Magick::Image("100x100", Magick::Color("transparent")); // blank.
	}

	// Turn the Icon pure white
	targetIcon.gamma(255);

	// Now make the Icon a little smaller and flip it right-side up （85/85 instead of 100/100)
	targetIcon.adaptiveResize(Magick::Geometry(targetIcon.size().width() * 0.85, targetIcon.size().height() * 0.85));
	targetIcon.flip();

	// Finally, combine the images into a new one
	baseFlag.composite(targetIcon, MagickCore::CenterGravity, MagickCore::OverCompositeOp);

	// Output
	baseFlag.write("flags.tmp/" + religion.name + "_rebels.tga");
}

void EU4::FlagFoundry::extendReligionStrips(const Configuration& theConfiguration, const std::vector<GeneratedReligion>& religions, const Mods& mods) const
{
	std::set<std::string> targetStrips = {"country_icon_religion.dds", "icon_religion.dds", "icon_religion_small.dds", "province_view_religion.dds"};
	for (const auto& religion: religions)
	{
		// Import the source dds
		Magick::Image sourceIcon;
		if (!religion.iconPath.empty())
		{
			auto foundIcon = false;
			auto iconPathDds = religion.iconPath;
			iconPathDds += ".dds";
			auto trimmedIconPath = religion.iconPath.filename().stem();
			trimmedIconPath += ".dds";
			auto overridePath = "configurables/gfx/custom_faith_icons" / trimmedIconPath; // one of these three should be it.
			auto path1 = theConfiguration.getCK3Path() / religion.iconPath;
			auto path2 = theConfiguration.getCK3Path() / "gfx/interface/icons/faith" / iconPathDds;
			if (commonItems::DoesFileExist(overridePath))
			{
				foundIcon = true;
				sourceIcon.read(overridePath.string());
			}
			if (!foundIcon && commonItems::DoesFileExist(path1))
			{
				foundIcon = true;
				sourceIcon.read(path1.string());
			}
			if (!foundIcon && commonItems::DoesFileExist(path2))
			{
				foundIcon = true;
				sourceIcon.read(path2.string());
			}
			if (!foundIcon)
			{
				for (const auto& mod: mods)
				{
					path1 = mod.path / religion.iconPath;
					path2 = mod.path / "gfx/interface/icons/faith" / iconPathDds;
					if (commonItems::DoesFileExist(path1))
					{
						foundIcon = true;
						sourceIcon.read(path1.string());
						break;
					}
					if (commonItems::DoesFileExist(path2))
					{
						foundIcon = true;
						sourceIcon.read(path2.string());
						break;
					}
				}
			}
			if (!foundIcon)
			{
				Log(LogLevel::Warning) << "Could not find religious icon: " << religion.iconPath.string() << ", skipping!";
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
			if (!commonItems::DoesFileExist("output" / theConfiguration.getOutputName() / "gfx/interface" / target))
				throw std::runtime_error("output/" + theConfiguration.getOutputName().string() + "/gfx/interface/" + target + "! Where are our religion strips?");
			Magick::Image targetStrip("output" + theConfiguration.getOutputName().string() + "/gfx/interface/" + target);
			targetStrip = extendReligionStrip(targetStrip, sourceIcon);
			targetStrip.write("output/" + theConfiguration.getOutputName().string() + "/gfx/interface/" + target);
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