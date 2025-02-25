#include "Settings.hpp"

Settings::Settings()
{
    DiscType A("A", sf::Color::Green, 5, 5);
    DiscType B("B", sf::Color::Red, 10, 10);
    DiscType C("C", sf::Color::Blue, 12, 12);
    DiscType D("D", sf::Color::Yellow, 15, 15);

    discTypeDistribution_[A] = 50;
    discTypeDistribution_[B] = 30;
    discTypeDistribution_[C] = 10;
    discTypeDistribution_[D] = 10;

    combinationReactionTable_[{A, B}] = {{C, 0.01f}, {D, 0.02f}};
    decompositionReactionTable_[C] = {{{A, B}, 0.01f}};
    decompositionReactionTable_[D] = {{{A, B}, 0.2f}};
}