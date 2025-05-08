#ifndef DIPLOMACY_H
#define DIPLOMACY_H

#include "Agreement.h"
#include "Parser.h"
#include <map>
#include <vector>

namespace EU4
{
class Country;
class Diplomacy: commonItems::parser
{
  public:
	Diplomacy();

	void addAgreement(std::shared_ptr<Agreement> agreement) { agreements.push_back(std::move(agreement)); }
	void importVassals(const std::map<std::string, std::shared_ptr<Country>>& countries);
	void generateTributaries(const std::map<std::string, std::shared_ptr<Country>>& countries);
	void updateTagsInAgreements(const std::string& oldTag, const std::string& newTag);
	void deleteAgreementsWithTag(const std::string& deadTag);
	void filterDeadRelationships(const std::map<std::string, std::shared_ptr<Country>>& countries);

	[[nodiscard]] const auto& getAgreements() const { return agreements; }
	[[nodiscard]] bool isCountryVassal(const std::string& tag) const;
	[[nodiscard]] bool isCountryJunior(const std::string& tag) const;

  private:
	void registerKeys();

	std::vector<std::shared_ptr<Agreement>> agreements;
};
} // namespace EU4

#endif // DIPLOMACY_H