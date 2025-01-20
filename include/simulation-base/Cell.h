#ifndef CELL_H
#define CELL_H

#include "Mitochondrium.h"
#include "Nucleus.h"
#include "Phases.h"

#include <memory>
#include <vector>

class Cell
{
public:
    typedef std::unique_ptr<Cell> Ptr;

public:
    Cell();

    Cell(const Cell&) = delete;

    auto operator=(const Cell&) -> Cell& = delete;

    auto mitochondria() const -> const std::vector<Mitochondrium::Ptr>&;

    auto nuclei() const -> const std::vector<Nucleus::Ptr>&;

    auto atpCount() const -> int;

    auto adpCount() const -> int;

    auto cellCyclePhase() const -> const CellCyclePhases;
    
private:
    void buildCell();

private:
    std::vector<Mitochondrium::Ptr> mitochondria_;
    std::vector<Nucleus::Ptr> nuclei_;

    const int StartATP = static_cast<int>(1e7f);
    int atpCount_ = StartATP;
    int adpCount_ = 0;
    CellCyclePhases cellCyclePhase_ = CellCyclePhases::Interphase;
};

#endif /* CELL_H */
