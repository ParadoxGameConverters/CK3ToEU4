#ifndef VERSION_PARSER_H
#define VERSION_PARSER_H



#include "Parser.h"



namespace mappers
{

class VersionParser: commonItems::parser
{
  public:
	VersionParser();
	explicit VersionParser(std::istream& theStream);

	friend std::ostream& operator<<(std::ostream& output, const VersionParser& versionParser);

  private:
	void registerKeys();

	std::string name;
	std::string version;
	std::string descriptionLine;
};

} // namespace mappers



#endif // VERSION_PARSER_H