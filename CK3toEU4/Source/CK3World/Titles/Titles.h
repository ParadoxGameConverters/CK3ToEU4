#ifndef CK3_TITLES_H
#define CK3_TITLES_H
#include "Parser.h"

namespace CK3
{
class Title;
class CoatsOfArms;
class Titles: commonItems::parser
{
  public:
	Titles() = default;
	explicit Titles(std::istream& theStream);

	[[nodiscard]] const auto& getTitles() const { return titles; }
	[[nodiscard]] const auto& getCounter() const { return titleCounter; }

	void linkCoats(const CoatsOfArms& coats);

  private:
	void registerKeys();

	std::vector<int> titleCounter = {0, 0, 0, 0, 0};
	std::map<std::string, std::shared_ptr<Title>> titles; // We're using NAME, not ID for key value!
};
} // namespace CK3

#endif // CK3_TITLES_H
