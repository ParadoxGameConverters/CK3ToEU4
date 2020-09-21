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

	Magick::Image pattern;
	// Pattern is the base of CoAs.
	if (coa.getPattern())
	{
		Log(LogLevel::Debug) << "Using pattern: " << *coa.getPattern();
		const auto patternSource = ck3Source + "patterns/" + *coa.getPattern();
		pattern.read(patternSource);
		if (coa.getColor1())
		{
			Log(LogLevel::Debug) << "Using color1: " << coa.getColor1()->outputRgb();
			pattern = recolorPattern(pattern, COLOR::COLOR1, *coa.getColor1());
		}
		if (coa.getColor2())
		{
			Log(LogLevel::Debug) << "Using color2: " << coa.getColor2()->outputRgb();
			pattern = recolorPattern(pattern, COLOR::COLOR2, *coa.getColor2());
		}
		if (coa.getColor3())
		{
			Log(LogLevel::Debug) << "Using color3: " << coa.getColor3()->outputRgb();
			pattern = recolorPattern(pattern, COLOR::COLOR3, *coa.getColor3());
		}
		image = pattern;
	}

	return image;
}

Magick::Image EU4::FlagCrafter::recolorPattern(const Magick::Image& pattern, COLOR colorcode, const commonItems::Color& color) const
{
	switch (colorcode)
	{
		case COLOR::COLOR1:
			return recolorImage(pattern, patternColorMasks.color1, color);		
		case COLOR::COLOR2:
			return recolorImage(pattern, patternColorMasks.color2, color);
		case COLOR::COLOR3:
			return recolorImage(pattern, patternColorMasks.color3, color);
		case COLOR::COLOR4: // not sure what to do here yet.
		case COLOR::COLOR5: // not sure what to do here yet.
			return pattern;
	}
	return pattern;
}
Magick::Image EU4::FlagCrafter::recolorEmblem(const Magick::Image& emblem, COLOR colorcode, const commonItems::Color& color) const
{
	switch (colorcode)
	{
		case COLOR::COLOR1:
			return recolorImage(emblem, emblemColorMasks.color1, color);
		case COLOR::COLOR2:
			return recolorImage(emblem, emblemColorMasks.color2, color);
		case COLOR::COLOR3:
			return recolorImage(emblem, emblemColorMasks.color3, commonItems::Color(std::array<int, 3>{255, 255, 255})); // rendering to white per PDX spec.
		case COLOR::COLOR4: // not sure what to do here yet.
		case COLOR::COLOR5: // not sure what to do here yet.
			return emblem;
	}
	return emblem;
}

bool EU4::FlagCrafter::ColorFuzzyEqual(const Magick::ColorRGB& a, const Magick::ColorRGB& b) const
{
	return std::fabs(a.red() - b.red()) < 0.01 && std::fabs(a.green() - b.green()) < 0.01 && std::fabs(a.blue() - b.blue()) < 0.01;
}

Magick::Image EU4::FlagCrafter::recolorImage(const Magick::Image& image, const commonItems::Color& mask, const commonItems::Color& color) const
{
	auto workingImage = image;
	workingImage.modifyImage();
	const size_t columns = workingImage.columns();
	const size_t rows = workingImage.rows();
	const Magick::ColorRGB fillColor(color.r()/255.0, color.g()/255.0, color.b()/255.0);
	const Magick::ColorRGB swapMask(mask.r() / 255.0, mask.g() / 255.0, mask.b() / 255.0);
	auto counter = 0;
	for (size_t row = 0; row < rows; ++row)
		for (size_t column = 0; column < columns; ++column)
		{
			const auto& px = Magick::ColorRGB(workingImage.pixelColor(column, row));
			if (ColorFuzzyEqual(px, swapMask))
			{
				workingImage.pixelColor(column, row, fillColor);
				++counter;
			}
		}
	workingImage.syncPixels();
	Log(LogLevel::Debug) << counter << " pixels replaced";
	return workingImage;
}
