#include "Warehouse.h"
#include "../../CK3World/CoatsOfArms/CoatOfArms.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"

Magick::Image EU4::Warehouse::getPattern(const CK3::CoatOfArms& coa)
{
	if (!coa.getPattern())
	{
		// This is a broken pattern. Return black image.
		Log(LogLevel::Warning) << "Coat of Arms is missing a pattern. Defaulting to black.";
		return Magick::Image(Magick::Geometry(128, 128), Magick::ColorRGB(0, 0, 0));
	}

	coloredImage newImageBlock; // This block is used both for comparing and for eventual storage.
	newImageBlock.color1 = coa.getColor1();
	newImageBlock.color2 = coa.getColor2();
	newImageBlock.color3 = coa.getColor3();
	newImageBlock.patternName = *coa.getPattern();

	Log(LogLevel::Debug) << "warehouse pattern enter on " << newImageBlock.patternName;
	
	// Do we have a coa like this stored?
	for (const auto& storedImage: patternStorage)
	{
		if (newImageBlock == storedImage)
		{
			Log(LogLevel::Debug) << "warehouse pattern match on cache";
			return storedImage.imageData; // We have a match of patternName and all possible colors.			
		}
	}

	// Do we have a base pattern in storage from which we can craft an image?
	if (!basePatterns.count(newImageBlock.patternName))
	{
		// Load the pattern from disk.
		if (!commonItems::DoesFileExist(imageFolder + "patterns/" + newImageBlock.patternName))
		{
			Log(LogLevel::Warning) << "Coat of arms uses invalid pattern: " << newImageBlock.patternName << ", defaulting to black.";
			return Magick::Image(Magick::Geometry(128, 128), Magick::ColorRGB(0, 0, 0));			
		}
		try
		{
			Log(LogLevel::Debug) << "warehouse loading pattern from disk: " << imageFolder + "patterns/" + newImageBlock.patternName;
			basePatterns.insert(std::pair(newImageBlock.patternName, Magick::Image(imageFolder + "patterns/" + newImageBlock.patternName)));
		}
		catch (std::exception& e)
		{
			Log(LogLevel::Warning) << "Coat of arms uses invalid pattern: " << newImageBlock.patternName << ", defaulting to black. (" << e.what() << ")";
			return Magick::Image(Magick::Geometry(128, 128), Magick::ColorRGB(0, 0, 0));
		}
	}

	newImageBlock.imageData = basePatterns[*coa.getPattern()];
	Log(LogLevel::Debug) << "pattern is sized: " << newImageBlock.imageData.size().width() << "x" << newImageBlock.imageData.size().height();

	// Send to coloring department.
	Log(LogLevel::Debug) << "warehouse sending pattern to recolorer";
	newImageBlock.imageData = recolorer.craftPatternImage(newImageBlock);

	// Store for posterity.
	patternStorage.emplace_back(newImageBlock);

	// And fulfill order
	Log(LogLevel::Debug) << "warehouse returning pattern";
	return newImageBlock.imageData;
}

std::vector<std::pair<CK3::Emblem, Magick::Image>> EU4::Warehouse::getColoredTextures(const std::vector<CK3::Emblem>& emblems)
{
	return getTextures(emblems, "colored_emblems");
}

std::vector<std::pair<CK3::Emblem, Magick::Image>> EU4::Warehouse::getTexturedTextures(const std::vector<CK3::Emblem>& emblems)
{
	return getTextures(emblems, "textured_emblems");
}

std::vector<std::pair<CK3::Emblem, Magick::Image>> EU4::Warehouse::getTextures(const std::vector<CK3::Emblem>& emblems, const std::string& directoryName)
{
	std::vector<std::pair<CK3::Emblem, Magick::Image>> toReturn;
	for (const auto& emblem: emblems)
	{
		if (!emblem.getTexture())
		{
			// This is a broken texture. Return black image.
			Log(LogLevel::Warning) << "Emblem is missing a texture. Defaulting to black.";
			toReturn.emplace_back(std::pair(emblem, Magick::Image(Magick::Geometry(128, 128), Magick::ColorRGB(0, 0, 0))));
			continue;
		}

		coloredImage newImageBlock; // This block is used both for comparing and for eventual storage.
		newImageBlock.color1 = emblem.getColor1();
		newImageBlock.color2 = emblem.getColor2();
		newImageBlock.color3 = emblem.getColor3();
		newImageBlock.patternName = *emblem.getTexture();

		Log(LogLevel::Debug) << "warehouse pattern enter on " << newImageBlock.patternName;

		// Do we have an emblem like this stored?
		auto matchedEmblem = false;
		if (directoryName == "colored_emblems")
			for (const auto& storedImage: coloredStorage)
			{
				if (newImageBlock == storedImage)
				{
					Log(LogLevel::Debug) << "warehouse colored texture match on cache";
					toReturn.emplace_back(std::pair(emblem, storedImage.imageData)); // We have a match of patternName and all possible colors.
					matchedEmblem = true;
					break;
				}
			}
		else if (directoryName == "textured_emblems")
			for (const auto& storedImage: texturedStorage)
			{
				if (newImageBlock == storedImage)
				{
					Log(LogLevel::Debug) << "warehouse textured texture match on cache";
					toReturn.emplace_back(std::pair(emblem, storedImage.imageData)); // We have a match of patternName and all possible colors.
					matchedEmblem = true;
					break;
				}
			}
		if (matchedEmblem)
			continue; // We're done here.

		// Do we have a base texture in storage from which we can craft an image?
		if (!baseTextures.count(newImageBlock.patternName))
		{
			// Load the texture from disk.
			if (!commonItems::DoesFileExist(imageFolder + directoryName + "/" + newImageBlock.patternName))
			{
				Log(LogLevel::Warning) << "Emblem uses invalid texture: " << newImageBlock.patternName << ", defaulting to black.";
				toReturn.emplace_back(std::pair(emblem, Magick::Image(Magick::Geometry(128, 128), Magick::ColorRGB(0, 0, 0))));
				continue;
			}
			try
			{
				Log(LogLevel::Debug) << "warehouse texture loading from disk: " << imageFolder + directoryName + "/" + newImageBlock.patternName;
				baseTextures.insert(std::pair(newImageBlock.patternName, Magick::Image(imageFolder + directoryName + "/" + newImageBlock.patternName)));
			}
			catch (std::exception& e)
			{
				Log(LogLevel::Warning) << "Emblem uses invalid texture: " << newImageBlock.patternName << ", defaulting to black. (" << e.what() << ")";
				toReturn.emplace_back(std::pair(emblem, Magick::Image(Magick::Geometry(128, 128), Magick::ColorRGB(0, 0, 0))));
				continue;
			}
		}

		newImageBlock.imageData = baseTextures[newImageBlock.patternName];
		Log(LogLevel::Debug) << "texture is sized: " << newImageBlock.imageData.size().width() << "x" << newImageBlock.imageData.size().height();

		// Send to coloring department.
		Log(LogLevel::Debug) << "warehouse texture sending to recolorer";
		newImageBlock.imageData = recolorer.craftTextureImage(newImageBlock);

		// Store for posterity.
		texturedStorage.emplace_back(newImageBlock);

		// And fulfill order
		toReturn.emplace_back(std::pair(emblem, newImageBlock.imageData));
	}
	Log(LogLevel::Debug) << "warehouse returning textures";
	return toReturn;
}

bool EU4::coloredImage::operator==(const coloredImage& rhs)
{
	if (patternName != rhs.patternName)
		return false;
	if (!color1 && rhs.color1 || color1 && !rhs.color1)
		return false;
	if (!color2 && rhs.color2 || color2 && !rhs.color2)
		return false;
	if (!color3 && rhs.color3 || color3 && !rhs.color3)
		return false;
	// Possible match, compare the colors.
	if (color1 && *color1 != *rhs.color1)
		return false;
	if (color2 && *color2 != *rhs.color2)
		return false;
	if (color3 && *color3 != *rhs.color3)
		return false;
	// Full match
	return true;
}
