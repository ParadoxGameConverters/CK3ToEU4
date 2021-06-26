#ifndef CONVERTER_VERSION_H
#define CONVERTER_VERSION_H
#include "GameVersion.h"
#include "Parser.h"

namespace mappers
{

class ConverterVersion: commonItems::convenientParser
{
  public:
	ConverterVersion();
	explicit ConverterVersion(std::istream& theStream);

	friend std::ostream& operator<<(std::ostream& output, const ConverterVersion& version);
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getVersion() const { return version; }
	[[nodiscard]] std::string getDescription() const;

	[[nodiscard]] const auto& getMinSource() const { return minSource; }
	[[nodiscard]] const auto& getMaxSource() const { return maxSource; }
	[[nodiscard]] const auto& getMinTarget() const { return minTarget; }
	[[nodiscard]] const auto& getMaxTarget() const { return maxTarget; }

  private:
	void registerKeys();

	std::string name;
	std::string version;
	std::string source;
	std::string target;
	GameVersion minSource;
	GameVersion maxSource;
	GameVersion minTarget;
	GameVersion maxTarget;
};

} // namespace mappers



#endif // CONVERTER_VERSION_H