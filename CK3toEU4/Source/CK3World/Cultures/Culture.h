#ifndef CK3_CULTURE_H
#define CK3_CULTURE_H
#include "Parser.h"

namespace CK3
{
class Culture: commonItems::parser
{
  public:
	Culture() = default;
	Culture(std::istream& theStream, long long theID);

	[[nodiscard]] auto getID() const { return ID; }
	[[nodiscard]] auto isCanonical() const { return canonical; }
	[[nodiscard]] std::string getName() const;

  private:
	void registerKeys();

	long long ID = 0;
	bool canonical = false;
	std::string culture_template;
	std::string name;
};
} // namespace CK3

#endif // CK3_CULTURE_H
