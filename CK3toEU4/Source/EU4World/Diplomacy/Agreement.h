#ifndef AGREEMENT_H
#define AGREEMENT_H

#include "Date.h"
#include "Parser.h"
#include <ostream>

namespace EU4
{
class Agreement: commonItems::parser
{
  public:
	Agreement() = default;
	Agreement(std::istream& theStream, std::string theType);
	Agreement(std::string _first, std::string _second, std::string _type, const date& _start_date):
		 type(std::move(_type)), first(std::move(_first)), second(std::move(_second)), startDate(_start_date) {}
	Agreement(std::string _first, std::string _second, std::string _type, std::string subject_type, const date& _start_date):
		 type(std::move(_type)), first(std::move(_first)), second(std::move(_second)), subjectType(std::move(subject_type)), startDate(_start_date) {}
	void updateTags(const std::string& oldTag, const std::string& newTag);

	[[nodiscard]] const auto& getFirst() const { return first; }
	[[nodiscard]] const auto& getSecond() const { return second; }
	[[nodiscard]] const auto& getType() const { return type; }

	friend std::ostream& operator<<(std::ostream& output, const Agreement& agreement);

  private:
	void registerKeys();

	std::string type;
	std::string first;
	std::string second;
	std::string subjectType;
	date startDate;
	date endDate;
};
} // namespace EU4

#endif // AGREEMENT_H