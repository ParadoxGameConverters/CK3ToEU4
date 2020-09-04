#ifndef CONVERTER_VERSION_H
#define CONVERTER_VERSION_H

#include "Parser.h"

namespace mappers
{

class ConverterVersion: commonItems::parser
{
  public:
	ConverterVersion();
	explicit ConverterVersion(std::istream& theStream);

	friend std::ostream& operator<<(std::ostream& output, const ConverterVersion& version);
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getVersion() const { return version; }
	[[nodiscard]] const auto& getDescription() const { return descriptionLine; }

  private:
	void registerKeys();

	std::string name;
	std::string version;
	std::string descriptionLine;
};

} // namespace mappers



#endif // CONVERTER_VERSION_H