#include "Faiths.h"
#include "Faith.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Faiths::Faiths(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Faiths::registerKeys()
{
	registerRegex(R"(\d+)", [this](const std::string& faithID, std::istream& theStream) {
		auto newFaith = std::make_shared<Faith>(theStream, std::stoi(faithID));
		faiths.insert(std::pair(newFaith->getID(), newFaith));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
