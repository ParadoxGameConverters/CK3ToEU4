
using commonItems;

namespace CK3ToEU4.CK3.Characters;

class CharacterDomain
{
	public CharacterDomain() {}
	public CharacterDomain(BufferedReader reader)
	{
		var parser = new Parser();
		registerKeys(parser);
		parser.ParseStream(reader);
	}

	public auto isPowerfulVassal() const { return powerfulVassal; }
	public auto getVassalStrength() const { return vassalStrength; }
	public const auto& getGovernment() const { return government; }
	public const auto& getLaws() const { return laws; }
	public const auto& getRealmCapital() const { return realmCapital; }
	public const auto& getDomain() const { return domain; }

	public void loadRealmCapital(const std::pair<long long, std::shared_ptr<Title>>& theRealmCapital) { realmCapital = theRealmCapital; }
	public void loadDomain(const std::vector<std::pair<long long, std::shared_ptr<Title>>>& theDomain) { domain = theDomain; }

	private void registerKeys()
	{
		registerKeyword("is_powerful_vassal", [this](const std::string& unused, std::istream& theStream) {
			powerfulVassal = commonItems::singleString(theStream).getString() == "yes";
		});
		registerKeyword("vassal_power_value", [this](const std::string& unused, std::istream& theStream) {
			vassalStrength = commonItems::singleDouble(theStream).getDouble();
		});
		registerKeyword("government", [this](const std::string& unused, std::istream& theStream) {
			government = commonItems::singleString(theStream).getString();
		});
		registerKeyword("laws", [this](const std::string& unused, std::istream& theStream) {
			const auto& lawsList = commonItems::stringList(theStream).getStrings();
			laws = std::set(lawsList.begin(), lawsList.end());
		});
		registerKeyword("realm_capital", [this](const std::string& unused, std::istream& theStream) {
			realmCapital = std::make_pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
		});
		registerKeyword("domain", [this](const std::string& unused, std::istream& theStream) {
			for (auto titleID: commonItems::llongList(theStream).getLlongs())
				domain.emplace_back(std::pair(titleID, nullptr));
		});
		registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	}


	private bool powerfulVassal = false;
	private double vassalStrength = 0;
	private std::string government;
	private std::set<std::string> laws;
	private std::pair<long long, std::shared_ptr<Title>> realmCapital;			// A barony!
	private std::vector<std::pair<long long, std::shared_ptr<Title>>> domain; // These are all titles owned (b-c-d-k-e), landless included. First one is PRIMARY.
}