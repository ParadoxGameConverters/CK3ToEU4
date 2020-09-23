#include "Recolorer.h"
#include "Magick++.h"
#include "Warehouse.h"

Magick::Image EU4::Recolorer::craftPatternImage(const coloredImage& imageBlock)
{
	/*
	 * Recoloring patters is not easy as mucking about with individual pixels using fuzzy matches results in crap.
	 * We approach this through channels. All patters have red at max, so we ignore red channel. Green channel inverse
	 * defines the actual red (as having blue over same pixels would result in magenta), and green channel positive
	 * gives us yellow color (for the same reason). We can use these green and green-inverse channels as alpha masks over
	 * color1 and color2 to create a clean composite.
	 *
	 * White, the third pattern color is where blue channel is activated. This means we can use blue channel as alpha over color3
	 * and collapse it on top of the first two layers for the final image.
	 *
	 * As a background we're using gray. There will be border pixels in some patterns where using forementioned alphas will result in
	 * non-100% solid color (70% red + 30% yellow is still 70% transparent in the end) so having a neutral color like grey helps there.
	 * Leaving it fully transparent is not possible as EU4 does not support transparency in its TGAs.
	 */
	
	// Create grey image for background.
	const auto geometry = Magick::Geometry(imageBlock.imageData.size().width(), imageBlock.imageData.size().height());
	auto workingImage = Magick::Image(geometry, Magick::ColorRGB(0.5, 0.5, 0.5));

	// We'll need the green, greenInverse and blue channels.
	auto greenChannel = imageBlock.imageData;
	greenChannel.channel(Magick::ChannelType::GreenChannel);
	auto greenChannelInvert = greenChannel;
	greenChannelInvert.negate();
	auto blueChannel = imageBlock.imageData;
	blueChannel.channel(Magick::ChannelType::BlueChannel);

	Magick::Image layer1; // ex-red. We use greeninvert as alpha over color1
	if (imageBlock.color1)
	{
		const auto color1 = Magick::ColorRGB(imageBlock.color1->r() / 255.0, imageBlock.color1->g() / 255.0, imageBlock.color1->b() / 255.0);
		layer1 = Magick::Image(geometry, color1);
		layer1.composite(greenChannelInvert, "0x0", MagickCore::CopyAlphaCompositeOp);
	}
	Magick::Image layer2; // ex=yellow. We use green as alpha over color2
	if (imageBlock.color2)
	{
		const auto color2 = Magick::ColorRGB(imageBlock.color2->r() / 255.0, imageBlock.color2->g() / 255.0, imageBlock.color2->b() / 255.0);
		layer2 = Magick::Image(geometry, color2);
		layer2.composite(greenChannel, "0x0", MagickCore::CopyAlphaCompositeOp);
	}
	Magick::Image layer3; // ex-white. We use blue as alpha over color3
	if (imageBlock.color3)
	{
		const auto color3 = Magick::ColorRGB(imageBlock.color3->r() / 255.0, imageBlock.color3->g() / 255.0, imageBlock.color3->b() / 255.0);
		layer3 = Magick::Image(geometry, color3);
		layer3.composite(blueChannel, "0x0", MagickCore::CopyAlphaCompositeOp);
	}

	// Now collapse all layers onto the background.
	if (imageBlock.color1)
		workingImage.composite(layer1, "0x0", MagickCore::OverCompositeOp);
	if (imageBlock.color2)
		workingImage.composite(layer2, "0x0", MagickCore::OverCompositeOp);
	if (imageBlock.color3)
		workingImage.composite(layer3, "0x0", MagickCore::OverCompositeOp);

	return workingImage;
}

Magick::Image EU4::Recolorer::craftTextureImage(const coloredImage& imageBlock)
{
	//return recolorImage(texture, replacementColors); // WIP
	return imageBlock.imageData;
}
