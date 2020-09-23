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
	// Get a background image
	Log(LogLevel::Debug) << "flagcrafter grabbing pattern";
	auto image = warehouse->getPattern(coa);
	// Get emblems

	Log(LogLevel::Debug) << "flagcrafter grabbing c emblems";
	auto coloredEmblems = warehouse->getColoredTextures(coa.getColoredEmblems());
	Log(LogLevel::Debug) << "flagcrafter grabbing t emblems";
	auto texturedEmblems = warehouse->getTexturedTextures(coa.getTexturedEmblems());

	Log(LogLevel::Debug) << "flagcrafter processing cemblems";
	image = processEmblemsOnImage(image, coloredEmblems);
	Log(LogLevel::Debug) << "flagcrafter processing temblems";
	image = processEmblemsOnImage(image, texturedEmblems);

	Log(LogLevel::Debug) << "flagcrafter done";
	return image;
}

Magick::Image EU4::FlagCrafter::processEmblemsOnImage(const Magick::Image& image, const std::vector<std::pair<CK3::Emblem, Magick::Image>>& emblems) const
{
	if (emblems.empty())
		return image;

	auto workingImage = image;

	for (const auto& emblemPair: emblems)
	{
		Log(LogLevel::Debug) << "flagcrafter in emblem " << *emblemPair.first.getTexture();
		if (emblemPair.first.getInstances().empty())
		{
			Log(LogLevel::Debug) << "flagcrafter generating single instance";
			// We need at least a nominal instance.
			CK3::EmblemInstance emblemInstance;
			std::vector<CK3::EmblemInstance> emblemVector = {emblemInstance};
			workingImage = imposeEmblemInstancesOnImage(workingImage, emblemVector, emblemPair.second);
		}
		else
		{
			// Run them all.
			Log(LogLevel::Debug) << "flagcrafter pushing existing instances";
			workingImage = imposeEmblemInstancesOnImage(workingImage, emblemPair.first.getInstances(), emblemPair.second);
		}
	}
	
	return workingImage;
}

Magick::Image EU4::FlagCrafter::imposeEmblemInstancesOnImage(const Magick::Image& image,
	 const std::vector<CK3::EmblemInstance>& instances,
	 const Magick::Image& emblem) const
{
	auto workingImage = image;
	workingImage.modifyImage();

	const auto height = workingImage.baseRows();
	const auto width = workingImage.baseColumns();
	
	for (const auto& instance: instances)
	{
		auto workingEmblem = image;
		workingEmblem.modifyImage();
		
		// Rescale emblem
		if (!instance.getScale().empty())
		{
			if (instance.getScale().size() != 2)
			{
				Log(LogLevel::Warning) << "Invalid emblem instance scale command, array size: " << instance.getScale().size();
				continue;
			}
			const auto targetWidth = instance.getScale()[0] * static_cast<double>(width);
			const auto targetHeight = instance.getScale()[1] * static_cast<double>(height);
			Log(LogLevel::Debug) << "flagcrafter scaling insance";
			workingEmblem.scale(Magick::Geometry(static_cast<size_t>(targetWidth), static_cast<size_t>(targetHeight)));
			Log(LogLevel::Debug) << "flagcrafter scaled instance";
		}
		
		// Rotate emblem
		if (instance.getRotation() == 0.0)
		{
			Log(LogLevel::Debug) << "flagcrafter rotating insance";
			workingEmblem.rotate(0 - instance.getRotation()); // I *think* PDX stores rotation clockwise.
			Log(LogLevel::Debug) << "flagcrafter rotated instance";
		}
		
		// Position emblem
		if (!instance.getPosition().empty()) // We won't paste anything without a position - which should only happen on user error.
		{
			if (instance.getPosition().size() != 2)
			{
				Log(LogLevel::Warning) << "Invalid emblem instance position command, array size: " << instance.getPosition().size();
				continue;
			}
			const auto targetX = static_cast<size_t>(instance.getPosition()[0] * static_cast<double>(width));
			const auto targetY = static_cast<size_t>(instance.getPosition()[1] * static_cast<double>(height));
			Log(LogLevel::Debug) << "flagcrafter compositing instance";
			workingImage.composite(workingEmblem, Magick::Geometry(targetX, targetY), MagickCore::BlendCompositeOp);
			Log(LogLevel::Debug) << "flagcrafter composited instance";
		}		
		
	}
	Log(LogLevel::Debug) << "flagcrafter returning completed instance run";
	return workingImage;
}