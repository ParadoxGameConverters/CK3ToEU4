
namespace mappers
{
class IAmHreMapper: commonItems::parser
{
  public:
	IAmHreMapper()
	{
		registerKeys();
		parseFile("configurables/i_am_hre.txt");
		clearRegisteredKeywords();
	}
	explicit IAmHreMapper(std::istream& theStream)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}

	[[nodiscard]] const auto& getHRE() const { return hre; }

  private:
	void registerKeys()
	{
		registerKeyword("hre_mechanics", [this](const std::string& unused, std::istream& theStream) {
			const commonItems::singleString hreStr(theStream);
			hre = hreStr.getString();
		});
		registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	}

	std::string hre;
};
}