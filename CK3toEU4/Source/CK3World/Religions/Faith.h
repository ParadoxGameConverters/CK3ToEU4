#ifndef CK3_FAITH_H
#define CK3_FAITH_H
#include "Parser.h"
#include "Color.h"
#include <set>

extern commonItems::Color::Factory laFabricaDeColor;

namespace CK3
{
class Religion;
class Faith: commonItems::parser
{
  public:
	Faith() = default;
	Faith(std::istream& theStream, int ID);

	[[nodiscard]] const auto& getName() const { return tag; }
	[[nodiscard]] const auto& getColor() const { return color; }
	[[nodiscard]] const auto& getDoctrines() const { return doctrines; }
	[[nodiscard]] const auto& getReligion() const { return religion; }
	[[nodiscard]] auto getID() const { return ID; }

	void loadReligion(const std::pair<int, std::shared_ptr<Religion>>& theReligion) { religion = theReligion; }

  private:
	void registerKeys();

	int ID = 0;
	std::string tag;
	std::optional<commonItems::Color> color;
	std::set<std::string> doctrines;
	std::pair<int, std::shared_ptr<Religion>> religion;
};
} // namespace CK3

#endif // CK3_FAITH_H
