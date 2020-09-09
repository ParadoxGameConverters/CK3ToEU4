#ifndef CK3_RELIGIONS_H
#define CK3_RELIGIONS_H
#include "Parser.h"
#include "Faiths.h"

namespace CK3
{
class Religion;
class Religions: commonItems::parser
{
  public:
	Religions() = default;
	explicit Religions(std::istream& theStream);

	[[nodiscard]] const auto& getReligions() const { return religions; }
	[[nodiscard]] auto getFaiths() { return std::move(faiths); }

  private:
	void registerKeys();

	std::map<int, std::shared_ptr<Religion>> religions;
	Faiths faiths;
};
} // namespace CK3

#endif // CK3_RELIGIONS_H
