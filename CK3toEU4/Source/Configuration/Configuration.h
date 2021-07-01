#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "ConverterVersion.h"
#include "Parser.h"
#include "ModLoader/ModLoader.h"

class Configuration: commonItems::parser
{
  public:
	Configuration() = default;
	explicit Configuration(const commonItems::ConverterVersion& converterVersion);
	explicit Configuration(std::istream& theStream);

	enum class STARTDATE
	{
		EU = 1,
		CK = 2
	};
	enum class SPLITVASSALS
	{
		YES = 1,
		NO = 2
	};
	enum class DEJURE
	{
		ENABLED = 1,
		DISABLED = 2
	};
	enum class DEVELOPMENT
	{
		IMPORT = 1,
		VANILLA = 2
	};
	enum class SUNSET
	{
		DEFAULT = 1,
		DISABLED = 2,
		ACTIVE = 3
	};
	enum class I_AM_HRE
	{
		HRE = 1,
		BYZANTIUM = 2,
		ROME = 3,
		CUSTOM = 4,
		NONE = 5
	};
	enum class SHATTER_EMPIRES
	{
		NONE = 1,
		ALL = 2,
		CUSTOM = 3
	};
	enum class SHATTER_LEVEL
	{
		DUTCHY = 1,
		KINGDOM = 2
	};
	enum class SHATTER_HRE_LEVEL
	{
		DUTCHY = 1,
		KINGDOM = 2
	};
	enum class SIBERIA
	{
		SMALL_SIBERIA = 1,
		CLEAR_SIBERIA = 2,
		LEAVE_SIBERIA = 3
	};
	enum class INSTITUTIONS
	{
		HISTORIC = 1,
		DYNAMIC = 2
	};

	[[nodiscard]] const auto& getSaveGamePath() const { return SaveGamePath; }
	[[nodiscard]] const auto& getCK3Path() const { return CK3Path; }
	[[nodiscard]] const auto& getCK3DocPath() const { return CK3DocPath; }
	[[nodiscard]] const auto& getEU4Path() const { return EU4Path; }
	[[nodiscard]] const auto& getOutputName() const { return outputName; }
	[[nodiscard]] const auto& getStartDate() const { return startDate; }
	[[nodiscard]] const auto& getHRE() const { return iAmHre; }
	[[nodiscard]] const auto& getShatterEmpires() const { return shatterEmpires; }
	[[nodiscard]] const auto& getShatterLevel() const { return shatterLevel; }
	[[nodiscard]] const auto& getDejure() const { return dejure; }
	[[nodiscard]] const auto& getShatterHRELevel() const { return shatterHRELevel; }
	[[nodiscard]] const auto& getSiberia() const { return siberia; }
	[[nodiscard]] const auto& getSunset() const { return sunset; }
	[[nodiscard]] const auto& getDynamicInstitutions() const { return dynamicInstitutions; }
	[[nodiscard]] const auto& getDevelopment() const { return development; }
	[[nodiscard]] const auto& getMods() const { return mods; }
	[[nodiscard]] const auto& getSplitVassals() const { return splitVassals; }

	void setMods(const Mods& theMods) { mods = theMods; }

  private:
	void registerKeys();
	void setOutputName();
	void verifyCK3Path();
	void verifyEU4Path() const;
	void verifyCK3Version(const commonItems::ConverterVersion& converterVersion) const;
	void verifyEU4Version(const commonItems::ConverterVersion& converterVersion) const;

	std::string SaveGamePath;
	std::string CK3Path;
	std::string CK3DocPath;
	std::string EU4Path;
	std::string outputName;

	STARTDATE startDate = STARTDATE::EU;
	I_AM_HRE iAmHre = I_AM_HRE::HRE;
	SHATTER_EMPIRES shatterEmpires = SHATTER_EMPIRES::NONE;
	SHATTER_HRE_LEVEL shatterHRELevel = SHATTER_HRE_LEVEL::DUTCHY;
	SHATTER_LEVEL shatterLevel = SHATTER_LEVEL::DUTCHY;
	SIBERIA siberia = SIBERIA::CLEAR_SIBERIA;
	SUNSET sunset = SUNSET::DEFAULT;
	INSTITUTIONS dynamicInstitutions = INSTITUTIONS::HISTORIC;
	DEVELOPMENT development = DEVELOPMENT::IMPORT;
	DEJURE dejure = DEJURE::ENABLED;
	SPLITVASSALS splitVassals = SPLITVASSALS::YES;

	Mods mods;
};

#endif // CONFIGURATION_H
