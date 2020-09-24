#include "FlagCrafter.h"
#include "../../CK3World/CoatsOfArms/CoatOfArms.h"
#include "../../CK3World/CoatsOfArms/Emblem.h"
#include "../../CK3World/CoatsOfArms/EmblemInstance.h"
#include "../Country/Country.h"
#include "Magick++.h"
#include "OSCompatibilityLayer.h"
#include "Warehouse.h"

/*
 * This class operates on image pairs. First of the two is a working image, one we slice and dice and copy stuff onto.
 * Second is the pristine pattern used to mask said stuff. We drag it around all the way from the warehouse to the forge
 * due to convenience of having it nearby and handy.
 */

Magick::Image EU4::FlagCrafter::craftFlagFromCoA(const CK3::CoatOfArms& coa) const
{
	// Probe for existing coa.
	if (coa.getID())
	{
		const auto probe = warehouse->getCoA(coa.getID());
		if (probe.first)
			return probe.second;		
	}

	// Crafting time.	Get a background image.
	auto imagePair = warehouse->getPattern(coa);
	// Grab subcoats
	auto counter = 0;
	std::vector<std::pair<CK3::CoatOfArms, Magick::Image>> subs;
	for (const auto& sub: coa.getSubs())
	{
		auto subimage = craftFlagFromCoA(*sub);
		subimage.write("subimage" + std::to_string(counter) + ".dds");
		++counter;
		subs.emplace_back(std::pair(*sub, subimage));
	}
	
	// Get emblems.
	const auto coloredEmblems = warehouse->getColoredTextures(coa.getColoredEmblems());
	const auto texturedEmblems = warehouse->getTexturedTextures(coa.getTexturedEmblems());

	imagePair = processSubsOnImage(imagePair, subs);
	imagePair = processEmblemsOnImage(imagePair, coloredEmblems);
	imagePair = processEmblemsOnImage(imagePair, texturedEmblems);

	if (coa.getID()) // subcoats won't have an ID.
		warehouse->storeCoA(coa.getID(), imagePair.first);
	
	return imagePair.first;
}

std::pair<Magick::Image, Magick::Image> EU4::FlagCrafter::processEmblemsOnImage(const std::pair<Magick::Image, Magick::Image>& imagePair,
	 const std::vector<std::pair<CK3::Emblem, Magick::Image>>& emblems) const
{
	if (emblems.empty())
		return imagePair;

	auto workingImage = imagePair;

	for (const auto& emblemPair: emblems)
	{
		if (emblemPair.first.getInstances().empty())
		{
			// We need at least a nominal emblem instance.
			CK3::EmblemInstance emblemInstance;
			emblemInstance.defaultPosition();
			std::vector<CK3::EmblemInstance> emblemVector = {emblemInstance};
			workingImage = imposeEmblemInstancesOnImage(workingImage, emblemVector, emblemPair.second, emblemPair.first.getMask());
		}
		else
		{
			// Run them all.
			workingImage = imposeEmblemInstancesOnImage(workingImage, emblemPair.first.getInstances(), emblemPair.second, emblemPair.first.getMask());
		}
	}

	return workingImage;
}

std::pair<Magick::Image, Magick::Image> EU4::FlagCrafter::processSubsOnImage(const std::pair<Magick::Image, Magick::Image>& imagePair,
	 const std::vector<std::pair<CK3::CoatOfArms, Magick::Image>>& subs) const
{
	if (subs.empty())
		return imagePair;

	auto workingImagePair = imagePair;

	for (const auto& subPair: subs)
	{
		if (subPair.first.getInstances().empty())
		{
			// We need at least a nominal emblem instance.
			CK3::EmblemInstance emblemInstance;
			emblemInstance.defaultOffset();
			std::vector<CK3::EmblemInstance> emblemVector = {emblemInstance};
			workingImagePair = imposeEmblemInstancesOnImage(workingImagePair, emblemVector, subPair.second, std::vector<int>{});
		}
		else
		{
			// Run them all.
			workingImagePair = imposeEmblemInstancesOnImage(workingImagePair, subPair.first.getInstances(), subPair.second, std::vector<int>{});
		}
	}

	return workingImagePair;
}


std::pair<Magick::Image, Magick::Image> EU4::FlagCrafter::imposeEmblemInstancesOnImage(const std::pair<Magick::Image, Magick::Image>& imagePair,
	 const std::vector<CK3::EmblemInstance>& instances,
	 const Magick::Image& emblem,
	 const std::vector<int>& masks) const
{
	auto workingImage = imagePair.first;

	const auto height = workingImage.baseRows();
	const auto width = workingImage.baseColumns();

	auto counter = 0;

	for (const auto& instance: instances)
	{
		counter++;
		auto workingEmblem = emblem;
		workingEmblem.write("origemblem.dds");

		// Rescale emblem
		if (!instance.getScale().empty())
		{
			if (instance.getScale().size() != 2)
			{
				Log(LogLevel::Warning) << "Invalid emblem instance scale command, array size: " << instance.getScale().size();
				continue;
			}
			auto scaleX = instance.getScale()[0];
			auto scaleY = instance.getScale()[1];
			if (scaleX < 0)
			{
				scaleX = 0 - scaleX;
				workingEmblem.flop();
			}
			if (scaleY < 0)
			{
				scaleY = 0 - scaleY;
				workingEmblem.flip();
			}
			const auto targetWidth = scaleX * static_cast<double>(width);
			const auto targetHeight = scaleY * static_cast<double>(height);
			workingEmblem.adaptiveResize(Magick::Geometry(static_cast<size_t>(targetWidth), static_cast<size_t>(targetHeight)));
			workingEmblem.write("scaledemblem" + std::to_string(counter) + ".dds");
		}

		// Rotate emblem
		if (instance.getRotation() != 0.0)
		{
			workingEmblem.backgroundColor(Magick::Color("none"));
			workingEmblem.rotate(instance.getRotation());
			workingEmblem.write("rotatedemblem" + std::to_string(counter) + ".dds");
		}

		// Position and clip emblem
		if (!instance.getPosition().empty()) // We won't paste anything without a position - which should only happen on user error.
		{
			if (instance.getPosition().size() != 2)
			{
				Log(LogLevel::Warning) << "Invalid emblem instance position command, array size: " << instance.getPosition().size();
				continue;
			}
			const auto targetX =
				 static_cast<size_t>(instance.getPosition()[0] * static_cast<double>(width) - static_cast<double>(workingEmblem.size().width()) / 2.0);
			const auto targetY =
				 static_cast<size_t>(instance.getPosition()[1] * static_cast<double>(height) - static_cast<double>(workingEmblem.size().height()) / 2.0);

			// Now it's time for a mask.
			if (masks.size() == 3)
			{
				// Masks are 3-int vectors containing nothing but { 0|1, 0|2, 0|3 }, telling us which color of original pattern to use to clip emblem.
				// We support any number of masks. The same emblem will be sliced 3 times if needed and portions pasted separately.
				for (auto mask: masks)
				{
					if (mask == 1)
					{
						// clip against red. Translation: Show only those parts of the emblem that match the pattern's original red.
						// We know this to be in fact greeninverse channel of the pattern.
						auto greenInverse = imagePair.second;
						greenInverse.channel(MagickCore::GreenChannel);
						greenInverse.negate();
						// Create a faux new image, transparent, and put emblem into proper position
						auto faux = Magick::Image(Magick::Geometry(workingImage.size().width(), workingImage.size().height()), Magick::Color("transparent"));
						faux.composite(workingEmblem, targetX, targetY, MagickCore::OverCompositeOp);						
						// Extract the alpha channel and exclude whatever is supposed to be masked
						auto fauxAlpha = faux;
						fauxAlpha.channel(MagickCore::AlphaChannel);
						fauxAlpha.composite(greenInverse, "0x0", MagickCore::DarkenCompositeOp);
						// Put the alpha channel back in
						faux.composite(fauxAlpha, "0x0", MagickCore::CopyAlphaCompositeOp);
						// And paste the faux layer onto the horse.
						workingImage.composite(faux, "0x0", MagickCore::OverCompositeOp);
					}
					else if (mask == 2)
					{
						// identical to before but with green channel.
						auto green = imagePair.second;
						green.channel(MagickCore::GreenChannel);
						auto faux = Magick::Image(Magick::Geometry(workingImage.size().width(), workingImage.size().height()), Magick::Color("transparent"));
						faux.composite(workingEmblem, targetX, targetY, MagickCore::OverCompositeOp);
						auto fauxAlpha = faux;
						fauxAlpha.channel(MagickCore::AlphaChannel);
						fauxAlpha.composite(green, "0x0", MagickCore::DarkenCompositeOp);
						faux.composite(fauxAlpha, "0x0", MagickCore::CopyAlphaCompositeOp);
						workingImage.composite(faux, "0x0", MagickCore::OverCompositeOp);
					}
					else if (mask == 3)
					{
						// ditto, blue channel.
						auto blue = imagePair.second;
						blue.channel(MagickCore::BlueChannel);
						auto faux = Magick::Image(Magick::Geometry(workingImage.size().width(), workingImage.size().height()), Magick::Color("transparent"));
						faux.composite(workingEmblem, targetX, targetY, MagickCore::OverCompositeOp);
						auto fauxAlpha = faux;
						fauxAlpha.channel(MagickCore::AlphaChannel);
						fauxAlpha.composite(blue, "0x0", MagickCore::DarkenCompositeOp);
						faux.composite(fauxAlpha, "0x0", MagickCore::CopyAlphaCompositeOp);
						workingImage.composite(faux, "0x0", MagickCore::OverCompositeOp);
					}
				}					
			}
			else
			{
				// No masks, straight clip.
				workingImage.composite(workingEmblem, targetX, targetY, MagickCore::OverCompositeOp);
				workingImage.write("workingemb" + std::to_string(counter) + ".dds");				
			}
			
		}
		
		// Position sub
		if (!instance.getOffset().empty()) // We won't paste anything without a position - which should only happen on user error.
		{
			if (instance.getOffset().size() != 2)
			{
				Log(LogLevel::Warning) << "Invalid emblem instance position command, array size: " << instance.getPosition().size();
				continue;
			}
			const auto targetX =
				 static_cast<size_t>(instance.getOffset()[0] * static_cast<double>(width));
			const auto targetY =
				 static_cast<size_t>(instance.getOffset()[1] * static_cast<double>(height));
			workingImage.composite(workingEmblem, targetX, targetY, MagickCore::OverCompositeOp);
			workingImage.write("workingsub" + std::to_string(counter) + ".dds");
		}

	}
	return std::pair(workingImage, imagePair.second);
}