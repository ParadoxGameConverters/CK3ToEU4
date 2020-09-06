#ifndef CK3_RELIGION_H
#define CK3_RELIGION_H
#include "Parser.h"

namespace CK3
{
class Faith;
class Religion: commonItems::parser
{
  public:
	Religion() = default;
	Religion(std::istream& theStream, int ID);

	[[nodiscard]] auto getID() const { return ID; }	
	[[nodiscard]] const auto& getName() const { return tag; }
	[[nodiscard]] const auto& getFamily() const { return family; }
	[[nodiscard]] const auto& getFaiths() const { return faiths; }

  private:
	void registerKeys();

	int ID = 0;
	std::string tag;
	std::string family;
	std::map<int, std::shared_ptr<Faith>> faiths;
};
} // namespace CK3

#endif // CK3_RELIGION_H
