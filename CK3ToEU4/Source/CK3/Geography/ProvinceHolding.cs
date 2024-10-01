
namespace CK3
{
class ProvinceHolding
{
	ProvinceHolding() = default;
	ProvinceHolding(BufferedReader reader)
	{
		registerKeys();
		parseStream(theStream);
		clearRegisteredKeywords();
	}

	public const auto& getHoldingType() const { return holdingType; }
	public const auto& getBuildings() const { return buildings; }
	public const auto& getSpecialBuilding() const { return specialBuilding; }
	public int countBuildings()
	{
		int totalBuildings = 0;
		for (const auto& building: buildings)
		{
			const auto pos = building.find_last_of("_");
			if (pos == string::npos)
				continue;
			const auto buildingString = building.substr(pos + 1, building.size());
			try
			{
				totalBuildings += std::stoi(buildingString);
			}
			catch (std::exception&)
			{
				Log(LogLevel::Warning) << "Province building stoi fail: " << buildingString << " from " << building;
				totalBuildings++;
			}
		}

		if (specialBuilding.empty())
			return totalBuildings;
		else
			return totalBuildings + 1;
	}


	private void registerKeys()
	{
		registerKeyword("holding", reader => {
			// A bit of recursion is good for sanity.
			const auto& holdingDetails = ProvinceHolding(theStream);
			holdingType = holdingDetails.getHoldingType();
			buildings = holdingDetails.getBuildings();
			specialBuilding = holdingDetails.getSpecialBuilding();
		});
		registerKeyword("type", reader => {
			holdingType = reader.GetString();
		});
		registerKeyword("special_building_type", reader => {
			specialBuilding = reader.GetString();
		});
		registerKeyword("buildings", reader => {
			// We'll get blobs, streamify them, and then again recursively parse them.
			const auto buildingBlobs = commonItems::blobList(theStream).getBlobs();
			for (const auto& blob: buildingBlobs)
			{
				auto blobStream = stringstream(blob);
				const auto newBuilding = ProvinceHolding(blobStream).getHoldingType();
				if (!newBuilding.empty())
					buildings.insert(newBuilding);
			}
		});
		registerRegex(CommonRegexes.Catchall, ParserHelpers.IgnoreItem);
	}

private string holdingType;
	private HashSet<string> buildings;
	private string specialBuilding;
};
}