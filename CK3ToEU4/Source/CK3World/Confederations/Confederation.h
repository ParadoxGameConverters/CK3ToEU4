#ifndef CK3_CONFEDERATION_H
#define CK3_CONFEDERATION_H
#include "Parser.h"
extern commonItems::Color::Factory laFabricaDeColor;

namespace CK3
{
class Character;
class CoatOfArms;

class Confederation: commonItems::parser
{
  public:
	Confederation() = default;
	Confederation(std::istream& theStream, long long theID);

	[[nodiscard]] auto getID() const { return ID; }
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getColor() const { return color; }
	[[nodiscard]] const auto& getCoat() const { return coat; }
	[[nodiscard]] const auto& getMembers() const { return members; }

	void loadMembers(const std::map<long long, std::shared_ptr<Character>>& theMembers) { members = theMembers; }
	void loadCoat(const std::pair<long long, std::shared_ptr<CoatOfArms>>& theCoat) { coat = theCoat; }

  private:
	void registerKeys();

	long long ID = 0;
	std::string name;
	std::optional<commonItems::Color> color;
	std::optional<std::pair<long long, std::shared_ptr<CoatOfArms>>> coat;
	std::map<long long, std::shared_ptr<Character>> members;
};
} // namespace CK3

#endif // CK3_CONFEDERATION_H
