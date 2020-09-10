#ifndef CK3_DYNASTY_H
#define CK3_DYNASTY_H
#include "Parser.h"

namespace CK3
{
class CoatOfArms;
class Dynasty: commonItems::parser
{
  public:
	Dynasty() = default;
	Dynasty(std::istream& theStream, int gameID);

	[[nodiscard]] auto getGameID() const { return gameID; }
	[[nodiscard]] auto isAppropriateRealmName() const { return appropriateRealmName; }
	[[nodiscard]] const auto& getDynID() const { return dynID; }
	[[nodiscard]] const auto& getCoA() const { return coa; }

	void loadCoat(const std::pair<int, std::shared_ptr<CoatOfArms>>& coat) { coa = coat; }

  private:
	void registerKeys();

	std::string dynID; // this is actual key, looks like an int but is actually string.
	int gameID = 0; // this is savegame key
	std::optional<std::pair<int, std::shared_ptr<CoatOfArms>>> coa;
	bool appropriateRealmName = false;
};
} // namespace CK3

#endif // CK3_DYNASTY_H
