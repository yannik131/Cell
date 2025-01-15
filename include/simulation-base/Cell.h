#ifndef CELL_H
#define CELL_H

#include "Mitochondrium.h"
#include "Nucleus.h"

#include <vector>
#include <memory>

class Cell {
public:
    typedef std::unique_ptr<Cell> Ptr;

    enum CellCyclePhases {
        Interphase,
        Mitosis
    };
    
    enum InterphasePhases {
        G0,
        G1,
        S,
        G2
    };
    
    enum MitosisPhases {
        Prophase,
        Prometaphase,
        Metaphase,
        Anaphase,
        Telophase,
        Cytokinesis
    };
    
public:
    Cell();
    
    Cell(const Cell&) = delete;
    
    const std::vector<Mitochondrium::Ptr>& mitochondria() const;
    
    const std::vector<Nucleus::Ptr>& nuclei() const;
    
    int atpCount() const;
    
    int adpCount() const;
    
    const CellCyclePhases cellCyclePhase() const;
    
private:
    std::vector<Mitochondrium::Ptr> mitochondria_;
    std::vector<Nucleus::Ptr> nuclei_;
    
    int atpCount_ = 1e7f;
    int adpCount_ = 0;
    CellCyclePhases cellCyclePhase_ = CellCyclePhases::Interphase;
};

#endif /* CELL_H */
