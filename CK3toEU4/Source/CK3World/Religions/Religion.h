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
	Religion(std::istream& theStream, long long theID);

	[[nodiscard]] auto getID() const { return ID; }	
	[[nodiscard]] const auto& getName() const { return tag; }
	[[nodiscard]] const auto& getFamily() const { return family; }
	[[nodiscard]] const auto& getFaiths() const { return faiths; }

	void loadFaiths(const std::map<long long, std::shared_ptr<Faith>>& theFaiths) { faiths = theFaiths; }	

  private:
	void registerKeys();

	long long ID = 0;
	std::string tag;
	std::string family;
	std::map<long long, std::shared_ptr<Faith>> faiths;
};
} // namespace CK3

#endif // CK3_RELIGION_H
