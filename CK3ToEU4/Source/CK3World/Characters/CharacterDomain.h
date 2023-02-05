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

	[[nodiscard]] auto isPowerfulVassal() const { return powerfulVassal; }
	[[nodiscard]] auto getVassalStrength() const { return vassalStrength; }
	[[nodiscard]] const auto& getGovernment() const { return government; }
	[[nodiscard]] const auto& getLaws() const { return laws; }
	[[nodiscard]] const auto& getRealmCapital() const { return realmCapital; }
	[[nodiscard]] const auto& getDomain() const { return domain; }

	void loadRealmCapital(const std::pair<long long, std::shared_ptr<Title>>& theRealmCapital) { realmCapital = theRealmCapital; }
	void loadDomain(const std::vector<std::pair<long long, std::shared_ptr<Title>>>& theDomain) { domain = theDomain; }

  private:
	void registerKeys();

	bool powerfulVassal = false;
	double vassalStrength = 0;
	std::string government;
	std::set<std::string> laws;
	std::pair<long long, std::shared_ptr<Title>> realmCapital;			// A barony!
	std::vector<std::pair<long long, std::shared_ptr<Title>>> domain; // These are all titles owned (b-c-d-k-e), landless included. First one is PRIMARY.
};
} // namespace CK3

#endif // CK3_CHARACTERDOMAIN_H
