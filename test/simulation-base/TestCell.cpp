#include "Cell.h"

#include <gtest/gtest.h>

TEST(CellTest, TestCellInitialization)
{
    Cell cell;

    // These are the start values a cell is equipped with
    ASSERT_EQ(cell.mitochondria().size(), 1);
    ASSERT_EQ(cell.nuclei().size(), 1);
    ASSERT_EQ(cell.atpCount(), 1e7);
    ASSERT_EQ(cell.adpCount(), 0);
    ASSERT_EQ(cell.cellCyclePhase(), Cell::CellCyclePhases::Interphase);
}