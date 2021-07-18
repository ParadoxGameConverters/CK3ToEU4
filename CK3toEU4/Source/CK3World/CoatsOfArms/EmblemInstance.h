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
	void defaultPosition() { position = {0.5, 0.5}; }
	void defaultOffset() { offset = {0.0, 0.0}; }

  private:
	void registerKeys();

	double rotation = 0.0;						 // Degrees, clockwise.
	double depth = 0.0;							 // Do. Not. Ask. Go to wiki and complain there.
	std::vector<double> position;				 // Default position is UPPER LEFT corner. Targets CENTER of emblem.
	std::vector<double> scale = {1.0, 1.0}; // Relative to image size. Defaults to "across entire image".
	std::vector<double> offset;				 // Used in sub-coat instances. Also UPPER LEFT corner. Targets UPPER LEFT corner of subcoat.
};
} // namespace CK3

#endif // CK3_EMBLEMINSTANCE_H
