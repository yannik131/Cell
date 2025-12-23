#include "core/PermeabilityMapping.hpp"
#include "core/Utility.hpp"

// TODO DRY violation with color mapping

namespace
{
using Permeability = cell::MembraneType::Permeability;
}

const QMap<cell::MembraneType::Permeability, QString>& getPermeabilityNameMapping()
{
    static const QMap<Permeability, QString> permeabilityNameMapping = {{Permeability::None, "None"},
                                                                        {Permeability::Inward, "Inward"},
                                                                        {Permeability::Outward, "Outward"},
                                                                        {Permeability::Bidirectional, "Bidirectional"}};

    return permeabilityNameMapping;
}

const QMap<QString, cell::MembraneType::Permeability>& getNamePermeabilityMapping()
{
    static const auto namePermeabilityMapping = utility::invertMap(getPermeabilityNameMapping());

    return namePermeabilityMapping;
}

const QList<cell::MembraneType::Permeability>& getSupportedPermeabilities()
{
    static const QList<Permeability> supportedPermeabilities = getPermeabilityNameMapping().keys();

    return supportedPermeabilities;
}

const QStringList& getSupportedPermeabilityNames()
{
    static const QStringList supportedPermeabilityNames = getPermeabilityNameMapping().values();

    return supportedPermeabilityNames;
}
