using CK3;

namespace CK3ToEU4.CK3.Cultures;

class Cultures: commonItems::parser
{
	public Cultures() {}
	public Cultures(std::istream& theStream)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}

	public const auto& getCultures() const { return cultures; }
	public void concoctCultures(const mappers::LocalizationMapper& localizationMapper, const mappers::CultureMapper& cultureMapper)
	{
		for (const auto& culture: cultures | std::views::values)
		{
			culture->concoctCultureName(localizationMapper, cultureMapper, cultureNamingCounter);
		}
	}


  
	private void registerKeys()
	{
		registerRegex(R"(\d+)", [this](const std::string& cultureID, std::istream& theStream) {
			auto newCulture = std::make_shared<Culture>(theStream, std::stoll(cultureID));
			cultures.insert(std::pair(newCulture->getID(), newCulture));
		});
		registerKeyword("cultures", [this](std::istream& theStream) {
			const auto scraper = Cultures(theStream);
			cultures = scraper.getCultures();
		});
		registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	}



	private std::map<long long, std::shared_ptr<Culture>> cultures;
	private std::map<std::string, int> cultureNamingCounter;
};