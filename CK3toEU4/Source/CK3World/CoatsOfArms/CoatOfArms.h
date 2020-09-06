#ifndef CK3_COATOFARMS_H
#define CK3_COATOFARMS_H
#include "Emblem.h"
#include "Parser.h"

namespace CK3
{
class CoatOfArms: commonItems::parser
{
  public:
	CoatOfArms() = default;
	CoatOfArms(std::istream& theStream, int ID);

	[[nodiscard]] auto getID() const { return ID; }
	[[nodiscard]] const auto& getPattern() const { return pattern; }
	[[nodiscard]] const auto& getColor1() const { return color1; }
	[[nodiscard]] const auto& getColor2() const { return color2; }
	[[nodiscard]] const auto& getColor3() const { return color3; }
	[[nodiscard]] const auto& getColor4() const { return color4; }
	[[nodiscard]] const auto& getColor5() const { return color5; }
	[[nodiscard]] const auto& getTexturedEmblems() const { return texturedEmblems; }
	[[nodiscard]] const auto& getColoredEmblems() const { return coloredEmblems; }
	[[nodiscard]] const auto& getSubs() const { return subs; }

  private:
	void registerKeys();

	int ID = 0;
	std::string pattern;
	std::string color1; // red
	std::string color2; // yellow
	std::string color3; // white
	std::string color4; // unknown. Black?
	std::string color5; // unknown
	std::vector<Emblem> texturedEmblems;
	std::vector<Emblem> coloredEmblems;
	std::vector<std::shared_ptr<CoatOfArms>> subs; // These are sub-coatsofarms. They are recursive!
	std::vector<EmblemInstance> instances;			  // Used to position subemblems.
};
} // namespace CK3

#endif // CK3_COATOFARMS_H
