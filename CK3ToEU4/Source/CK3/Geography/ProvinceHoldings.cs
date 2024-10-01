
using CK3;

namespace CK3ToEU4.CK3.Geography;

class ProvinceHoldings
{
  
	public ProvinceHoldings() {}
	public ProvinceHoldings(BufferedReader reader)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}

	public const auto& getProvinceHoldings() const { return provinceHoldings; }

	private void registerKeys()
	{
		registerRegex(R"(\d+)", [this](const string& baronyID, std::istream& theStream) {
			auto newBarony = std::make_shared<ProvinceHolding>(theStream);
			provinceHoldings.insert(std::pair(std::stoi(baronyID), newBarony));
		});
		registerRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}


	private Dictionary<int, std::shared_ptr<ProvinceHolding>> provinceHoldings;
}