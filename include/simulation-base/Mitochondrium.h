#ifndef MITOCHONDRIUM_H
#define MITOCHONDRIUM_H

#include <memory>

class Cell;

class Mitochondrium {
public:
    typedef std::unique_ptr<Mitochondrium> Ptr;
    
public:
    Mitochondrium(Cell* parentCell);
    
    Mitochondrium(const Mitochondrium&) = delete;
    
private:
    Cell* parentCell_;
};

#endif /* MITOCHONDRIUM_H */
