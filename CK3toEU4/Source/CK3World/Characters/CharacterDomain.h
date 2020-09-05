#ifndef CK3_CHARACTERDOMAIN_H
#define CK3_CHARACTERDOMAIN_H
#include "Date.h"
#include "Parser.h"
#include <set>

namespace CK3
{
class Title;
class CharacterDomain: commonItems::parser
{
  public:
	CharacterDomain() = default;
	CharacterDomain(std::istream& theStream);

	[[nodiscard]] const auto& getDomain() const { return domain; }
	[[nodiscard]] const auto& getLaws() const { return laws; }
	[[nodiscard]] const auto& getGovernment() const { return government; }
	[[nodiscard]] const auto& getRealmCapital() const { return realmCapital; }
	[[nodiscard]] auto isPowerfulVassal() const { return powerfulVassal; }
	[[nodiscard]] auto getVassalStrength() const { return vassalStrength; }

  private:
	void registerKeys();

	std::map<int, std::shared_ptr<Title>> domain; // These are BARONIES directly owned. They need to be mapped to something usable.
	std::set<std::string> laws;
	bool powerfulVassal = false;
	double vassalStrength = 0;
	std::string government;
	std::pair<int, std::shared_ptr<Title>> realmCapital; // Again, a barony!
};
} // namespace CK3

#endif // CK3_CHARACTERDOMAIN_H
