#ifndef CK3_CONFEDERATIONS_H
#define CK3_CONFEDERATIONS_H
#include "Parser.h"

namespace CK3
{
class CoatsOfArms;
class Confederation;
class Characters;
class Confederations: commonItems::parser
{
  public:
	Confederations() = default;
	explicit Confederations(std::istream& theStream);

	void linkCharacters(const Characters& characters);
	void linkCoats(const CoatsOfArms& coats);

	[[nodiscard]] const auto& getConfederations() const { return confederations; }

  private:
	void registerKeys();
	parser databaseParser;

	std::map<long long, std::shared_ptr<Confederation>> confederations;
};
} // namespace CK3

#endif // CK3_CONFEDERATIONS_H
