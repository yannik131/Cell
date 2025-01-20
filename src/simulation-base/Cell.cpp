#include "Cell.h"

Cell::Cell()
{
    buildCell();
}

auto Cell::mitochondria() const -> const std::vector<Mitochondrium::Ptr>&
{
    return mitochondria_;
}

auto Cell::nuclei() const -> const std::vector<Nucleus::Ptr>&
{
    return nuclei_;
}

auto Cell::atpCount() const -> int
{
    return atpCount_;
}

auto Cell::adpCount() const -> int
{
    return adpCount_;
}

auto Cell::cellCyclePhase() const -> const Cell::CellCyclePhases
{
    return cellCyclePhase_;
}

void Cell::buildCell()
{
    mitochondria_.push_back(std::make_unique<Mitochondrium>(this));
    nuclei_.push_back(std::make_unique<Nucleus>(this));
}
