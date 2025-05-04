#ifndef CK3_CONFEDERATIONS_H
#define CK3_CONFEDERATIONS_H
#include "Parser.h"

namespace CK3
{
class Confederation;
class Confederations: commonItems::parser
{
  public:
	Confederations() = default;
	explicit Confederations(std::istream& theStream);

	[[nodiscard]] const auto& getConfederations() const { return confederations; }

  private:
	void registerKeys();
	parser databaseParser;

	std::map<long long, std::shared_ptr<Confederation>> confederations;
};
} // namespace CK3

#endif // CK3_CONFEDERATIONS_H
