

namespace CK3
{
class Religion;
class Faith: commonItems::parser
{
  public:
	Faith() = default;
	Faith(std::istream& theStream, long long theID): ID(theID)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}

	[[nodiscard]] const auto& getName() const { return tag; }
	[[nodiscard]] const auto& getColor() const { return color; }
	[[nodiscard]] const auto& getDoctrines() const { return doctrines; }
	[[nodiscard]] const auto& getReligion() const { return religion; }
	[[nodiscard]] const auto& getReligiousHead() const { return religiousHead; }
	[[nodiscard]] auto getID() const { return ID; }
	[[nodiscard]] const auto& getCustomName() const { return customName; }
	[[nodiscard]] const auto& getCustomAdj() const { return customAdjective; }
	[[nodiscard]] const auto& getDescription() const { return description; }
	[[nodiscard]] const auto& getTemplate() const { return religionTemplate; }
	[[nodiscard]] const auto& getIconPath() const { return iconPath; }
	[[nodiscard]] const auto& getReformedFlag() const { return reformedFlag; }

	void setReligiousHead(const auto& newHead) { religiousHead = newHead; }
	void loadReligion(const std::pair<long long, std::shared_ptr<Religion>>& theReligion) { religion = theReligion; }

  private:
	void registerKeys()
	{
		registerKeyword("tag", [this](const std::string& unused, std::istream& theStream) {
			tag = commonItems::singleString(theStream).getString();
		});
		registerKeyword("doctrine", [this](const std::string& unused, std::istream& theStream) {
			doctrines.emplace_back(commonItems::singleString(theStream).getString());
		});
		registerKeyword("religion", [this](const std::string& unused, std::istream& theStream) {
			religion = std::make_pair(commonItems::singleLlong(theStream).getLlong(), nullptr);
		});
		registerKeyword("color", [this](const std::string& unused, std::istream& theStream) {
			color = laFabricaDeColor.getColor(theStream);
		});
		registerKeyword("template", [this](const std::string& unused, std::istream& theStream) {
			religionTemplate = commonItems::singleString(theStream).getString();
		});
		registerKeyword("name", [this](const std::string& unused, std::istream& theStream) {
			customName = commonItems::singleString(theStream).getString();
		});
		registerKeyword("adjective", [this](const std::string& unused, std::istream& theStream) {
			customAdjective = commonItems::singleString(theStream).getString();
		});
		registerKeyword("religious_head", [this](const std::string& unused, std::istream& theStream) {
			religiousHead = commonItems::singleString(theStream).getString();
		});
		registerKeyword("desc", [this](const std::string& unused, std::istream& theStream) {
			description = commonItems::singleString(theStream).getString();
		});
		registerKeyword("icon", [this](const std::string& unused, std::istream& theStream) {
			iconPath = commonItems::singleString(theStream).getString();
		});
		registerKeyword("variables", [this](const std::string& unused, std::istream& theStream) {
			if (commonItems::stringOfItem(theStream).getString().find("has_been_reformed") != std::string::npos)
				reformedFlag = true;
		});
		registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	}


	bool reformedFlag = false;
	long long ID = 0;
	std::string tag;
	std::string religionTemplate;
	std::string iconPath;
	std::string customName;
	std::string customAdjective;
	std::string description;
	std::string religiousHead;
	std::optional<commonItems::Color> color;
	std::vector<std::string>
		 doctrines; // This is a vector in order to keep order consistent. We want the first things read (tenets) to be the first things output, ALWAYS
	std::pair<long long, std::shared_ptr<Religion>> religion;
};
}