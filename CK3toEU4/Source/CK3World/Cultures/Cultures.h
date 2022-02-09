#ifndef CK3_CULTURES_H
#define CK3_CULTURES_H
#include "Parser.h"
#include "../../Mappers/LocalizationMapper/LocalizationMapper.h"
#include "../../Mappers/CultureMapper/CultureMapper.h"

namespace CK3
{
class Culture;
class Cultures: commonItems::parser
{
  public:
	Cultures() = default;
	explicit Cultures(std::istream& theStream);

	[[nodiscard]] const auto& getCultures() const { return cultures; }
	void concoctCultures(const mappers::LocalizationMapper& localizationMapper, const mappers::CultureMapper& cultureMapper);

  private:
	void registerKeys();

	std::map<long long, std::shared_ptr<Culture>> cultures;
	std::map<std::string, int> cultureNamingCounter;
};
} // namespace CK3

#endif // CK3_CULTURES_H
