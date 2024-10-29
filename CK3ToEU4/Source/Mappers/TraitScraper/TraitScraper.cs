namespace CK3ToEU4.Mappers.TraitScraper;

class TraitScraper
{
	public TraitScraper() {}
	public void loadTraits(std::istream& theStream)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}
	public void loadTraits(const std::string& fileName)
	{
		registerKeys();
		parseFile(fileName);
		clearRegisteredKeywords();
	}

	public const auto& getTraits() const { return traits; }

	public std::optional<std::string> getTraitForID(int ID) const
	{
		const auto& traitsItr = traits.find(ID);
		if (traitsItr != traits.end())
			return traitsItr->second;
		return std::nullopt;
	}


	private void registerKeys()
	{
		registerRegex("@.*", commonItems::ignoreItem);
		registerRegex(commonItems::catchallRegex, [this](const std::string& traitName, std::istream& theStream) {
			const auto newScraping = TraitScraping(theStream);
			if (newScraping.getIndex() > 0) // indexes start at 1!
			{
				if (traits.contains(newScraping.getIndex()))
				{
					traits.at(newScraping.getIndex()) = traitName; // override existing one
				}
				else
				{
					traits.emplace(newScraping.getIndex(), traitName);
				}
			}
			else // otherwise index them ourselves.
			{
				if (!traits.contains(index))
				{
					traits.emplace(index, traitName);
				}
				else
				{
					traits.at(index) = traitName; // override existing one
				}
				++index;
			}
		});
	}

	private int index = 1;
	private std::map<int, std::string> traits;
}