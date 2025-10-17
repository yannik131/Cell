#ifndef F75C4C88_E1B8_44E1_8648_594CBA398805_HPP
#define F75C4C88_E1B8_44E1_8648_594CBA398805_HPP

#include "cell/MembraneType.hpp"

#include <QList>
#include <QMap>

const QMap<cell::MembraneType::Permeability, QString>& getPermeabilityNameMapping();

const QMap<QString, cell::MembraneType::Permeability>& getNamePermeabilityMapping();

const QList<cell::MembraneType::Permeability>& getSupportedPermeabilities();

const QStringList& getSupportedPermeabilityNames();

#endif /* F75C4C88_E1B8_44E1_8648_594CBA398805_HPP */
