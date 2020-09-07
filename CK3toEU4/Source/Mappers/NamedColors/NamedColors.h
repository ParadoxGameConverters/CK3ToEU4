#ifndef NAMED_COLORS_H
#define NAMED_COLORS_H
#include "Parser.h"
#include "Color.h"
extern commonItems::Color::Factory laFabricaDeColor;

namespace mappers
{
class NamedColors: commonItems::parser
{
  public:
	NamedColors() = default;
	void loadColors(std::istream& theStream);
	void loadColors(const std::string& filepath);

  private:
	void registerKeys();
};
} // namespace mappers

#endif // NAMED_COLORS_H