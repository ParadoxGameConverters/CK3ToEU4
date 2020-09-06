#ifndef CK3_EMBLEM_H
#define CK3_EMBLEM_H
#include "EmblemInstance.h"
#include "Parser.h"

namespace CK3
{
class Emblem: commonItems::parser
{
  public:
	Emblem() = default;
	Emblem(std::istream& theStream);

	[[nodiscard]] const auto& getTexture() const { return texture; }
	[[nodiscard]] const auto& getColor1() const { return color1; }
	[[nodiscard]] const auto& getColor2() const { return color2; }
	[[nodiscard]] const auto& getColor3() const { return color3; }
	[[nodiscard]] const auto& getMask() const { return mask; }
	[[nodiscard]] const auto& getInstances() const { return instances; }

  private:
	void registerKeys();

	std::string texture;
	std::string color1; // blue
	std::string color2; // green
	std::string color3; // magenta - unsupported by PDX, must render to white!
	std::vector<int> mask;
	std::vector<EmblemInstance> instances;
};
} // namespace CK3

#endif // CK3_EMBLEM_H
