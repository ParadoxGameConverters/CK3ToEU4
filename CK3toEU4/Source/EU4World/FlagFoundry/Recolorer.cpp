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
	/*
	 * Recoloring textures is similar to patterns but channel names are different. Unlike patterns, most textures
	 * are monochromatic with the blue channel serving as brightness mask - the darker blue the darker the resulting final
	 * composite should be, with neutral being b=127 and b=255 should make entire emblem texture white regardless of colors.
	 *
	 * greeninverse and green are again used for color1 and color2 respectively, and color3 is gained via red channel this time.
	 * Texture itself - the shape - is in fact stored in its alpha channel.
	 *
	 * Once we composite the layers we need to apply first the blue channel as a contrast filter and then the original alpha channel
	 * to get the shape we need.
	 */
	const auto geometry = Magick::Geometry(imageBlock.imageData.size().width(), imageBlock.imageData.size().height());
	auto workingImage = Magick::Image(geometry, Magick::ColorRGB(0.5, 0.5, 0.5));

	// We'll need the green, greenInverse and red channels.
	auto greenChannel = imageBlock.imageData;
	greenChannel.channel(Magick::ChannelType::GreenChannel);
	auto greenChannelInvert = greenChannel;
	greenChannelInvert.negate();
	auto blueChannel = imageBlock.imageData;
	blueChannel.channel(Magick::ChannelType::BlueChannel);
	auto redChannel = imageBlock.imageData;
	redChannel.channel(Magick::ChannelType::RedChannel);
	auto alphaChannel = imageBlock.imageData;
	alphaChannel.channel(Magick::ChannelType::AlphaChannel);

	Magick::Image layer1; // ex-blue127. We use greeninvert as alpha over color1
	if (imageBlock.color1)
	{
		const auto color1 = Magick::ColorRGB(imageBlock.color1->r() / 255.0, imageBlock.color1->g() / 255.0, imageBlock.color1->b() / 255.0);
		layer1 = Magick::Image(geometry, color1);
		layer1.composite(greenChannelInvert, "0x0", MagickCore::CopyAlphaCompositeOp);
	}
	Magick::Image layer2; // ex-greenish (g255b127). We use green as alpha over color2
	if (imageBlock.color2)
	{
		const auto color2 = Magick::ColorRGB(imageBlock.color2->r() / 255.0, imageBlock.color2->g() / 255.0, imageBlock.color2->b() / 255.0);
		layer2 = Magick::Image(geometry, color2);
		layer2.composite(greenChannel, "0x0", MagickCore::CopyAlphaCompositeOp);
	}
	Magick::Image layer3; // ex-magentaish (r255b127). We use red as alpha over color3
	if (imageBlock.color3)
	{
		const auto color3 = Magick::ColorRGB(imageBlock.color3->r() / 255.0, imageBlock.color3->g() / 255.0, imageBlock.color3->b() / 255.0);
		layer3 = Magick::Image(geometry, color3);
		layer3.composite(redChannel, "0x0", MagickCore::CopyAlphaCompositeOp);
	}

	// Now collapse all layers onto the background.
	if (imageBlock.color1)
		workingImage.composite(layer1, "0x0", MagickCore::OverCompositeOp);
	if (imageBlock.color2)
		workingImage.composite(layer2, "0x0", MagickCore::OverCompositeOp);
	if (imageBlock.color3)
		workingImage.composite(layer3, "0x0", MagickCore::OverCompositeOp);

	// Apply blue channel as brightness mask. This takes some doing.
	// Shave off only negatives (below blue 127)
	auto tempMask = blueChannel;
	tempMask.composite(blueChannel, "0x0", MagickCore::PlusCompositeOp); // doubling the blue channel, this is now almost entirely white with darker blotches.
	workingImage.composite(tempMask, "0x0", MagickCore::MultiplyCompositeOp); // My multiplying, the dark blotches are applied to source
	// Now shave off only positives
	const auto gray = Magick::Image(geometry, Magick::ColorRGB(0.5, 0.5, 0.5));
	tempMask = blueChannel;
	tempMask.composite(gray, "0x0", MagickCore::MinusSrcCompositeOp);				// minus once, to get to black, but we need to double the scale.
	tempMask.composite(tempMask, "0x0", MagickCore::PlusCompositeOp);				// this is now almost entirely black with whiter blotches.
	tempMask.negate();																			// back to white with darker blotches where we should brighten the image
	workingImage.composite(tempMask, "0x0", MagickCore::DivideSrcCompositeOp); // And by dividing, we're brightening those darker pixels from mask.

	// Apply original alpha
	workingImage.composite(alphaChannel, "0x0", MagickCore::CopyAlphaCompositeOp);

	return workingImage;
}
