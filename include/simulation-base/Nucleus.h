#ifndef NUCLEUS_H
#define NUCLEUS_H

#include "Phases.h"

#include <memory>

class Cell;

class Nucleus
{
public:
    typedef std::unique_ptr<Nucleus> Ptr;

public:
    Nucleus(Cell* parentCell);

    Nucleus(const Nucleus&) = delete;

private:
    Cell* parentCell_;
    
    
};

#endif /* NUCLEUS_H */
