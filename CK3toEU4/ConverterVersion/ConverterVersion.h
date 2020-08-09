#ifndef CONVERTER_VERSION_H
#define CONVERTER_VERSION_H



#include "Parser.h"



namespace ConverterVersion
{

class ConverterVersion: commonItems::parser
{
	public:
	ConverterVersion();
	explicit ConverterVersion(std::istream& theStream);

	friend std::ostream& operator<<(std::ostream& output, const ConverterVersion& version);

	private:
	void registerKeys();

	std::string name;
	std::string version;
	std::string descriptionLine;
};

} // namespace mappers



#endif // CONVERTER_VERSION_H