#include "FlagCrafter.h"
#include "../../CK3World/CoatsOfArms/CoatOfArms.h"
#include "../../CK3World/CoatsOfArms/Emblem.h"
#include "../../CK3World/CoatsOfArms/EmblemInstance.h"
#include "../Country/Country.h"
#include "Magick++.h"
#include "OSCompatibilityLayer.h"
#include "Warehouse.h"

Magick::Image EU4::FlagCrafter::craftFlagFromCoA(const CK3::CoatOfArms& coa) const
{
	// Probe for existing coa.
	Log(LogLevel::Debug) << "Enter cffcoa";
	if (coa.getID())
	{
		Log(LogLevel::Debug) << "Probing";
		const auto probe = warehouse->getCoA(coa.getID());
		if (probe.first)
			return probe.second;		
	}

	// Crafting time.	Get a background image.
	auto image = warehouse->getPattern(coa);
	Log(LogLevel::Debug) << "got pattern";
	// Grab subcoats
	auto counter = 0;
	std::vector<std::pair<CK3::CoatOfArms, Magick::Image>> subs;
	for (const auto& sub: coa.getSubs())
	{
		Log(LogLevel::Debug) << "forsub";
		auto subimage = craftFlagFromCoA(*sub);
		subimage.write("subimage" + std::to_string(counter) + ".dds");
		++counter;
		Log(LogLevel::Debug) << "gotsub";
		subs.emplace_back(std::pair(*sub, subimage));
	}
	
	// Get emblems.
	const auto coloredEmblems = warehouse->getColoredTextures(coa.getColoredEmblems());
	const auto texturedEmblems = warehouse->getTexturedTextures(coa.getTexturedEmblems());

		Log(LogLevel::Debug) << "go sub";
	image = processSubsOnImage(image, subs);
	image = processEmblemsOnImage(image, coloredEmblems);
	image = processEmblemsOnImage(image, texturedEmblems);

	if (coa.getID()) // subcoats won't have an ID.
		warehouse->storeCoA(coa.getID(), image);
	
	return image;
}

Magick::Image EU4::FlagCrafter::processEmblemsOnImage(const Magick::Image& image, const std::vector<std::pair<CK3::Emblem, Magick::Image>>& emblems) const
{
	if (emblems.empty())
		return image;

	auto workingImage = image;

	for (const auto& emblemPair: emblems)
	{
		if (emblemPair.first.getInstances().empty())
		{
			// We need at least a nominal emblem instance.
			CK3::EmblemInstance emblemInstance;
			emblemInstance.defaultPosition();
			std::vector<CK3::EmblemInstance> emblemVector = {emblemInstance};
			workingImage = imposeEmblemInstancesOnImage(workingImage, emblemVector, emblemPair.second);
		}
		else
		{
			// Run them all.
			workingImage = imposeEmblemInstancesOnImage(workingImage, emblemPair.first.getInstances(), emblemPair.second);
		}
	}

	return workingImage;
}

Magick::Image EU4::FlagCrafter::processSubsOnImage(const Magick::Image& image, const std::vector<std::pair<CK3::CoatOfArms, Magick::Image>>& subs) const
{
	if (subs.empty())
		return image;

	auto workingImage = image;

	for (const auto& subPair: subs)
	{
		if (subPair.first.getInstances().empty())
		{
			// We need at least a nominal emblem instance.
			CK3::EmblemInstance emblemInstance;
			emblemInstance.defaultOffset();
			std::vector<CK3::EmblemInstance> emblemVector = {emblemInstance};
			workingImage = imposeEmblemInstancesOnImage(workingImage, emblemVector, subPair.second);
		}
		else
		{
			// Run them all.
			workingImage = imposeEmblemInstancesOnImage(workingImage, subPair.first.getInstances(), subPair.second);
		}
	}

	return workingImage;
}


Magick::Image EU4::FlagCrafter::imposeEmblemInstancesOnImage(const Magick::Image& image,
	 const std::vector<CK3::EmblemInstance>& instances,
	 const Magick::Image& emblem) const
{
	auto workingImage = image;

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

		// Position emblem
		if (!instance.getPosition().empty()) // We won't paste anything without a position - which should only happen on user error.
		{
			if (instance.getPosition().size() != 2)
			{
				Log(LogLevel::Warning) << "Invalid emblem instance position command, array size: " << instance.getPosition().size();
				continue;
			}
			Log(LogLevel::Debug) << "instance position " << instance.getPosition()[0] << " by " << instance.getPosition()[1];
			Log(LogLevel::Debug) << "working size " << workingEmblem.size().width() << " by " << workingEmblem.size().height();
			const auto targetX =
				 static_cast<size_t>(instance.getPosition()[0] * static_cast<double>(width) - static_cast<double>(workingEmblem.size().width()) / 2.0);
			const auto targetY =
				 static_cast<size_t>(instance.getPosition()[1] * static_cast<double>(height) - static_cast<double>(workingEmblem.size().height()) / 2.0);
			Log(LogLevel::Debug) << "position " << targetX << " by " << targetY;
			workingImage.composite(workingEmblem, targetX, targetY, MagickCore::OverCompositeOp);
			workingImage.write("workingemb" + std::to_string(counter) + ".dds");
		}
		
		// Position sub
		if (!instance.getOffset().empty()) // We won't paste anything without a position - which should only happen on user error.
		{
			if (instance.getOffset().size() != 2)
			{
				Log(LogLevel::Warning) << "Invalid emblem instance position command, array size: " << instance.getPosition().size();
				continue;
			}
			Log(LogLevel::Debug) << "instance offset " << instance.getOffset()[0] << " by " << instance.getOffset()[1];
			Log(LogLevel::Debug) << "working size " << workingEmblem.size().width() << " by " << workingEmblem.size().height();
			const auto targetX =
				 static_cast<size_t>(instance.getOffset()[0] * static_cast<double>(width));
			const auto targetY =
				 static_cast<size_t>(instance.getOffset()[1] * static_cast<double>(height));
			Log(LogLevel::Debug) << "offset " << targetX << " by " << targetY;
			workingImage.composite(workingEmblem, targetX, targetY, MagickCore::OverCompositeOp);
			workingImage.write("workingsub" + std::to_string(counter) + ".dds");
		}

	}
	return workingImage;
}