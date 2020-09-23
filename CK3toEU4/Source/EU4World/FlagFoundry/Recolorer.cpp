#include "Recolorer.h"
#include "Log.h"
#include "Magick++.h"
#include "Warehouse.h"

Magick::Image EU4::Recolorer::craftPatternImage(const coloredImage& imageBlock) const
{
	return craftImage(imageBlock, IMAGETYPE::PATTERN);
}

Magick::Image EU4::Recolorer::craftTextureImage(const coloredImage& imageBlock) const
{
	return craftImage(imageBlock, IMAGETYPE::TEXTURE);
}

Magick::Image EU4::Recolorer::craftImage(const coloredImage& imageBlock, IMAGETYPE imageType) const
{
	Log(LogLevel::Debug) << "Recolorer using path: " << imageBlock.patternName;

	std::vector<std::pair<COLOR, commonItems::Color>> replacementMatrix;
	if (imageBlock.color1)
	{
		replacementMatrix.emplace_back(std::pair(COLOR::COLOR1, *imageBlock.color1));
		Log(LogLevel::Debug) << "Recolorer using color1: " << *imageBlock.color1;
	}
	if (imageBlock.color2)
	{
		replacementMatrix.emplace_back(std::pair(COLOR::COLOR2, *imageBlock.color2));
		Log(LogLevel::Debug) << "Recolorer using color2: " << *imageBlock.color2;
	}
	if (imageBlock.color3)
	{
		replacementMatrix.emplace_back(std::pair(COLOR::COLOR3, *imageBlock.color3));
		Log(LogLevel::Debug) << "Recolorer using color3: " << *imageBlock.color3;
	}

	Log(LogLevel::Debug) << "recolorer recoloring image";
	switch (imageType)
	{
		case IMAGETYPE::PATTERN:
			Log(LogLevel::Debug) << "recolorer returning pattern";
			return recolorPattern(imageBlock.imageData, replacementMatrix);
		case IMAGETYPE::TEXTURE:
			Log(LogLevel::Debug) << "recolorer returning texture";
			return recolorTexture(imageBlock.imageData, replacementMatrix);
	}
	// Silly codacy, but also a fallback if something goes wrong.
	Log(LogLevel::Debug) << "recolorer defaulting";
	return Magick::Image(Magick::Geometry(128, 128), Magick::ColorRGB(0, 0, 0));
}

Magick::Image EU4::Recolorer::recolorPattern(const Magick::Image& pattern, const std::vector<std::pair<COLOR, commonItems::Color>>& replacementMatrix) const
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
	Log(LogLevel::Debug) << "recolorer forwarding pattern with colors: " << replacementColors.size();
	return recolorImage(pattern, replacementColors);
}

Magick::Image EU4::Recolorer::recolorTexture(const Magick::Image& texture, const std::vector<std::pair<COLOR, commonItems::Color>>& replacementMatrix) const
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
				replacementColors.emplace_back(
					 std::pair(emblemColorMasks.color3, commonItems::Color(std::array<int, 3>{255, 255, 255}))); // rendering to white per PDX spec.
				break;
			case COLOR::COLOR4: // not sure what to do here yet.
			case COLOR::COLOR5: // not sure what to do here yet.
				break;
		}
	Log(LogLevel::Debug) << "recolorer forwarding texture with colors: " << replacementColors.size();
	return recolorImage(texture, replacementColors);
}

Magick::Image EU4::Recolorer::recolorImage(const Magick::Image& image,
	 const std::vector<std::pair<commonItems::Color, commonItems::Color>>& replacementColors) const
{
	auto workingImage = image;
	workingImage.modifyImage(); // Lock image from outside influences.
	// I *think* this means colors distanced by "25" (in rgb ints maybe) are considered equal for purposes of replacement.
	workingImage.colorFuzz(25.0 * 65535 / 100.0);

	for (const auto& color: replacementColors)
	{
		// color to be replaced
		const Magick::ColorRGB swapMask(color.first.r() / 255.0, color.first.g() / 255.0, color.first.b() / 255.0);

		// color that replaces it
		const Magick::ColorRGB fillColor(color.second.r() / 255.0, color.second.g() / 255.0, color.second.b() / 255.0);

		// and.... Magick++
		workingImage.opaque(swapMask, fillColor);
	}

	workingImage.flip();
	return workingImage;
}
