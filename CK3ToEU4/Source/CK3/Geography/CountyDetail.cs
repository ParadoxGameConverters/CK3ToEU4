
namespace CK3ToEU4.CK3.Geography;

class CountyDetail
{
	public CountyDetail() {};
	public CountyDetail(std::istream& theStream)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}

	public auto getDevelopment() const { return development; }
	public const auto& getCulture() const { return culture; }
	public const auto& getFaith() const { return faith; }
	public const auto& isDeJureHRE() const { return deJureHRE; }

	public void loadCulture(const std::pair<long long, std::shared_ptr<Culture>>& theCulture) { culture = theCulture; }
	public void loadFaith(const std::pair<long long, std::shared_ptr<Faith>>& theFaith) { faith = theFaith; }
	public void setDeJureHRE() { deJureHRE = true; }

	private void registerKeys()
	{
		registerKeyword("development", [this](const std::string& unused, std::istream& theStream) {
			development = commonItems::singleInt(theStream).getInt();
		});
		registerKeyword("culture", [this](const std::string& unused, std::istream& theStream) {
			culture = std::make_pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
			if (culture.first == 4294967295)
				culture.first = 0;
		});
		registerKeyword("faith", [this](const std::string& unused, std::istream& theStream) {
			faith = std::make_pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
			if (faith.first == 4294967295)
				faith.first = 0;
		});
		registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	}


	private int development = 0;
	private std::pair<long long, std::shared_ptr<Culture>> culture;
	private std::pair<long long, std::shared_ptr<Faith>> faith;
	private bool deJureHRE = false;
}