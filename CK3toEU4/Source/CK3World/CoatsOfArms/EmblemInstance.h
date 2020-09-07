#ifndef CK3_EMBLEMINSTANCE_H
#define CK3_EMBLEMINSTANCE_H
#include "Parser.h"

namespace CK3
{
class EmblemInstance: commonItems::parser
{
  public:
	EmblemInstance() = default;
	EmblemInstance(std::istream& theStream);

	[[nodiscard]] auto getRotation() const { return rotation; }
	[[nodiscard]] auto getDepth() const { return depth; }
	[[nodiscard]] const auto& getPosition() const { return position; }
	[[nodiscard]] const auto& getScale() const { return scale; }
	[[nodiscard]] const auto& getOffset() const { return offset; }

  private:
	void registerKeys();

	double rotation = 0;
	double depth = 0; // Do. Not. Ask. Go to wiki and complain there.
	std::vector<double> position;
	std::vector<double> scale;
	std::vector<double> offset; // Used in sub-emblem instances
};
} // namespace CK3

#endif // CK3_EMBLEMINSTANCE_H
