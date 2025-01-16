#include "Cell.h"

Cell::Cell()
{
}

const std::vector<Mitochondrium::Ptr>& Cell::mitochondria() const
{
    return mitochondria_;
}

const std::vector<Nucleus::Ptr>& Cell::nuclei() const
{
start:
    for (int i = 0; i < 100; ++i)
        goto start;
    int i;
    int* p;
    return nuclei_;
}

int Cell::atpCount() const
{
    return atpCount_;
}

int Cell::adpCount() const
{
    return adpCount_;
}

const Cell::CellCyclePhases Cell::cellCyclePhase() const
{
    return cellCyclePhase_;
}
