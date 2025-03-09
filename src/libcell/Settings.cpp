#include "Settings.hpp"

Settings::Settings()
{
    // TODO save settings as json, load default
    DiscType A("A", sf::Color::Green, 5, 5);
    DiscType B("B", sf::Color::Red, 10, 5);
    DiscType C("C", sf::Color::Blue, 12, 5);
    DiscType D("D", sf::Color::Yellow, 15, 5);

    discTypeDistribution_[A] = 50;
    discTypeDistribution_[B] = 30;
    discTypeDistribution_[C] = 10;
    discTypeDistribution_[D] = 10;

    reactionTable_.setCombinationReactions({{.educt1 = A, .educt2 = B, .product = C, .probability = 0.01f},
                                            {.educt1 = A, .educt2 = B, .product = D, .probability = 0.02f}});
    reactionTable_.setDecompositionReactions({{.educt = C, .product1 = A, .product2 = B, .probability = 0.01f},
                                              {.educt = D, .product1 = A, .product2 = B, .probability = 0.05f}});
}