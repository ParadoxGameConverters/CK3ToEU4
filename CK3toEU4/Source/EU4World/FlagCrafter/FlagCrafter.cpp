#include "FlagCrafter.h"
#include "../../CK3World/CoatsOfArms/CoatOfArms.h"
#include "../../CK3World/CoatsOfArms/Emblem.h"
#include "../../CK3World/CoatsOfArms/EmblemInstance.h"
#include "../../CK3World/Dynasties/House.h"
#include "../../CK3World/Titles/Title.h"
#include "../../Configuration/Configuration.h"
#include "../Country/Country.h"
#include "Magick++.h"
#include "OSCompatibilityLayer.h"
#include <cmath>

// We are calling this class and its methods outside of outWorld. This is because CK3toEU4Tests depends on outWorld and
// we do not want to pollute tests build with imagemagick's shenanigans. So, we will generate flags in a temporary directory
// and then move it to flags folder in outWorld.

// This means this is the only class that interacts with imageMagick, outside of EU4World, which depends on this one.

void EU4::FlagCrafter::generateFlags(const std::map<std::string, std::shared_ptr<Country>>& countries, const Configuration& theConfiguration)
{
	ck3Source = theConfiguration.getCK3Path() + "gfx/coat_of_arms/";
	
	// prep the battleground.
	if (!commonItems::DeleteFolder("flags.tmp"))
		throw std::runtime_error("Could not delete flags.tmp folder!");
	else if (!commonItems::TryCreateFolder("flags.tmp"))
		throw std::runtime_error("Could not create flags.tmp folder!");

	// For every tag we must have a flag. This can be vanilla flag (in which case we do nothing), or
	// our pre-generated flag (which we copy over) or a dynamic flag from CK3 savegame which we have yet to create.

	for (const auto& country: countries)
	{
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

void EU4::FlagCrafter::craftFlag(const std::shared_ptr<Country>& country) const
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
	Log(LogLevel::Debug) << "crafting: " << country->getTag() << " from " << country->getTitle()->second->getName();
	auto generatedCoa = craftFlagFromCoA(*coa);
	try
	{
		generatedCoa.magick("TGA");
	}
	catch (std::exception& e)
	{
		Log(LogLevel::Error) << "Failed setting flag format: " << e.what();
	}
	try
	{
		generatedCoa.write("flags.tmp/" + country->getTag() + ".tga");
	}
	catch (std::exception& e)
	{
		Log(LogLevel::Error) << "Failed writing flag to disk: " << e.what();
	}


	Log(LogLevel::Debug) << "Crafted: " << country->getTag() << ".tga";
}

Magick::Image EU4::FlagCrafter::craftFlagFromCoA(const CK3::CoatOfArms& coa) const
{
	// Create a blank (black) 128x128 image.
	Magick::Image image("128x128", "black");

	Magick::Image pattern; // Pattern is the base of CoAs.
	std::vector<std::pair<COLOR, commonItems::Color>> replacementMatrix;	
	if (coa.getPattern())
	{
		Log(LogLevel::Debug) << "Using pattern: " << *coa.getPattern();
		pattern.read(ck3Source + "patterns/" + *coa.getPattern());
		if (coa.getColor1())
		{
			replacementMatrix.emplace_back(std::pair(COLOR::COLOR1, *coa.getColor1()));
			Log(LogLevel::Debug) << "Using color1: " << coa.getColor1()->outputRgb();
		}
		if (coa.getColor2())
		{
			replacementMatrix.emplace_back(std::pair(COLOR::COLOR2, *coa.getColor2()));
			Log(LogLevel::Debug) << "Using color2: " << coa.getColor2()->outputRgb();
		}
		if (coa.getColor3())
		{
			replacementMatrix.emplace_back(std::pair(COLOR::COLOR3, *coa.getColor3()));
			Log(LogLevel::Debug) << "Using color3: " << coa.getColor3()->outputRgb();
		}
		pattern = recolorPattern(pattern, replacementMatrix);
		image = pattern;
	}

	return image;
}

Magick::Image EU4::FlagCrafter::recolorPattern(const Magick::Image& pattern, const std::vector<std::pair<COLOR, commonItems::Color>>& replacementMatrix) const
{
	std::vector<std::pair<commonItems::Color, commonItems::Color>> replacementColors;
	for (const auto& entry: replacementMatrix)
		switch (entry.first)
		{
			case COLOR::COLOR1:
				replacementColors.emplace_back(std::pair(patternColorMasks.color1, entry.second));
				break;
			case COLOR::COLOR2:
				replacementColors.emplace_back(std::pair(patternColorMasks.color2, entry.second));
				break;
			case COLOR::COLOR3:
				replacementColors.emplace_back(std::pair(patternColorMasks.color3, entry.second));
				break;
			case COLOR::COLOR4: // not sure what to do here yet.
			case COLOR::COLOR5: // not sure what to do here yet.
				break;
		}
	return recolorImage(pattern, replacementColors);
}
Magick::Image EU4::FlagCrafter::recolorEmblem(const Magick::Image& emblem, const std::vector<std::pair<COLOR, commonItems::Color>>& replacementMatrix) const
{
	std::vector<std::pair<commonItems::Color, commonItems::Color>> replacementColors;
	for (const auto& entry: replacementMatrix)
		switch (entry.first)
		{
			case COLOR::COLOR1:
				replacementColors.emplace_back(std::pair(emblemColorMasks.color1, entry.second));
				break;
			case COLOR::COLOR2:
				replacementColors.emplace_back(std::pair(emblemColorMasks.color2, entry.second));
				break;
			case COLOR::COLOR3:
				replacementColors.emplace_back(std::pair(emblemColorMasks.color3, commonItems::Color(std::array<int, 3>{255, 255, 255})));// rendering to white per PDX spec.
				break;
			case COLOR::COLOR4: // not sure what to do here yet.
			case COLOR::COLOR5: // not sure what to do here yet.
				break;
		}
	return recolorImage(emblem, replacementColors);
}

bool EU4::FlagCrafter::ColorFuzzyEqual(const Magick::ColorRGB& a, const Magick::ColorRGB& b) const
{
	return std::fabs(a.red() - b.red()) < 0.01 && std::fabs(a.green() - b.green()) < 0.01 && std::fabs(a.blue() - b.blue()) < 0.01;
}

bool EU4::FlagCrafter::ColorFuzzyNearby(const Magick::ColorRGB& a, const Magick::ColorRGB& b) const
{
	return std::sqrt(std::pow(a.red() - b.red(), 2) + std::pow(a.green() - b.green(), 2)  + std::pow(a.blue() - b.blue(), 2)) < 0.25;
}

Magick::Image EU4::FlagCrafter::recolorImage(const Magick::Image& image,
	 const std::vector<std::pair<commonItems::Color, commonItems::Color>>& replacementColors) const
{
	auto workingImage = image;
	workingImage.modifyImage();
	workingImage.colorFuzz(25.0 * 65535 / 100.0);

	for (const auto& replacement: replacementColors)
	{
		const Magick::ColorRGB swapMask(replacement.first.r() / 255.0, replacement.first.g() / 255.0, replacement.first.b() / 255.0); // color to be replaced
		const Magick::ColorRGB fillColor(replacement.second.r() / 255.0,
			 replacement.second.g() / 255.0,
			 replacement.second.b() / 255.0); // color that replaces it
		workingImage.opaque(swapMask, fillColor);
	}

	workingImage.flip();
	return workingImage;
}
