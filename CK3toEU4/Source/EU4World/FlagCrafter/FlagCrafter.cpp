#include "FlagCrafter.h"
#include "Magick++.h"

void EU4::FlagCrafter::test() const
{
	Magick::Image image("test.dds");
	image.magick("TGA");
	image.write("test.tga");
}
