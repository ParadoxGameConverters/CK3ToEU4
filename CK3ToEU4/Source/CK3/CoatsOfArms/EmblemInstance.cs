using System.Collections.Generic;
using commonItems;

namespace CK3ToEU4.CK3.CoatsOfArms;

public class EmblemInstance
{
    public EmblemInstance(BufferedReader reader)
    {
        var parser = new Parser();
        RegisterKeys(parser);
        parser.ParseStream(reader);
    }
    
    private void RegisterKeys(Parser parser)
    {
        registerKeyword("rotation", [this](const std::string& unused, std::istream& theStream) {
            rotation = commonItems::singleDouble(theStream).getDouble();
        });
        registerKeyword("depth", [this](const std::string& unused, std::istream& theStream) {
            depth = commonItems::singleDouble(theStream).getDouble();
        });
        registerKeyword("position", [this](const std::string& unused, std::istream& theStream) {
            position = commonItems::doubleList(theStream).getDoubles();
        });
        registerKeyword("scale", [this](const std::string& unused, std::istream& theStream) {
            scale = commonItems::doubleList(theStream).getDoubles();
        });
        registerKeyword("offset", [this](const std::string& unused, std::istream& theStream) {
            offset = commonItems::doubleList(theStream).getDoubles();
        });
        registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
    }

    public void DefaultPosition()
    {
        position = [0.5, 0.5];
    }
    public void DefaultOffset()
    {
        offset = [0.0, 0.0];
    }

    public double Rotation { get; private set; } = 0.0;	// Degrees, clockwise.
    public double Depth { get; private set; } = 0.0;	// Do. Not. Ask. Go to wiki and complain there.
    public IReadOnlyList<double> Position => position;	// Default position is UPPER LEFT corner. Targets CENTER of emblem.
    private List<double> position = [];
    public IReadOnlyList<double> Scale => scale;        // Relative to image size. Defaults to "across entire image".
    private List<double> scale = [1.0, 1.0];
    public IReadOnlyList<double> Offset => offset;      // Used in sub-coat instances. Also UPPER LEFT corner. Targets UPPER LEFT corner of subcoat.
    private List<double> offset = [];

}