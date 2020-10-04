#ifndef GENERATED_RELIGION_H
#define GENERATED_RELIGION_H

#include "Color.h"
#include <ostream>

namespace EU4
{
	struct GeneratedReligion
	{
		void outputRebels(std::ostream& output) const;

		std::string country;
		std::string countrySecondary;
		std::string province;
		std::string unique;
		std::string nonUnique;
		std::string iconPath;
		std::string religionGroup;
		std::string staticBlob;
		std::string name;
		std::string allowedConversion;
		std::string parent;
		std::optional<commonItems::Color> color;
		int icon = 0;
		friend std::ostream& operator<<(std::ostream& output, const GeneratedReligion& religion);
	};
} // namespace EU4

#endif // GENERATED_RELIGION_H