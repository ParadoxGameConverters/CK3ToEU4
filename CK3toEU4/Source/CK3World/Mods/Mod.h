#ifndef CK3_MOD_H
#define CK3_MOD_H

#include "Parser.h"

namespace CK3
{
class Mod: commonItems::parser
{
  public:
	explicit Mod(std::istream& theStream);

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getPath() const { return path; }
	[[nodiscard]] auto looksValid() const { return !name.empty() && !path.empty(); }
	[[nodiscard]] auto isCompressed() const { return compressed; }

	void setPath(const std::string& thePath) { path = thePath; }
	
  private:
	bool compressed = false;

	std::string name;
	std::string path;
};
} // namespace CK3

#endif // CK3_MOD_H