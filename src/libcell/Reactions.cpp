#include "Reactions.hpp"
#include "Disc.hpp"
#include "GlobalSettings.hpp"
#include "MathUtils.hpp"

bool combinationReaction(Disc* d1, Disc* d2)
{
    const auto& combinationReactionTable = GlobalSettings::getSettings().combinationReactions_;
    auto iter = combinationReactionTable.find(std::make_pair(d2->getType(), d1->getType()));

    if (iter == combinationReactionTable.end())
        return false;

    const auto& possibleReactions = iter->second;
    float randomNumber = MathUtils::getRandomFloat();
    for (const auto& reaction : possibleReactions)
    {
        if (randomNumber > reaction.getProbability())
            continue;

        const auto& resultType = reaction.getProduct1();

        // For reactions of type A + B -> C, we keep the one closer in size to C and destroy the other
        if (std::abs(resultType.getRadius() - d1->getType().getRadius()) >
            std::abs(resultType.getRadius() - d2->getType().getRadius()))
            std::swap(d1, d2);

        d1->setVelocity((d1->getType().getMass() * d1->getVelocity() + d2->getType().getMass() * d2->getVelocity()) /
                        resultType.getMass());
        d1->setType(resultType);

        d2->markDestroyed();

        return true;
    }

    return false;
}

bool exchangeReaction(Disc* d1, Disc* d2)
{
    // TODO Probabilities of exchange and combination reactions should together add up to 100%
    const auto& settings = GlobalSettings::getSettings();

    const auto& exchangeReactionTable = settings.exchangeReactions_;
    auto iter = exchangeReactionTable.find(std::make_pair(d2->getType(), d1->getType()));

    if (iter == exchangeReactionTable.end())
        return false;

    const auto& possibleReactions = iter->second;
    float randomNumber = MathUtils::getRandomFloat();
    for (const auto& reaction : possibleReactions)
    {
        if (randomNumber > reaction.getProbability())
            continue;

        d1->scaleVelocity(std::sqrt(d1->getType().getMass() / reaction.getProduct1().getMass()));
        d1->setType(reaction.getProduct1());

        d2->scaleVelocity(std::sqrt(d2->getType().getMass() / reaction.getProduct2().getMass()));
        d2->setType(reaction.getProduct2());

        return true;
    }

    return false;
}

void decompositionReaction(Disc* d1, std::vector<Disc>& newDiscs)
{
    const auto& decompositionReactionTable = GlobalSettings::getSettings().decompositionReactions_;
    const float& dt = GlobalSettings::getSettings().simulationTimeStep_.asSeconds();

    const auto& iter = decompositionReactionTable.find(d1->getType());
    if (iter == decompositionReactionTable.end())
        return;

    const auto& possibleReactions = iter->second;
    float randomNumber = MathUtils::getRandomFloat();
    for (const auto& reaction : possibleReactions)
    {
        if (randomNumber > 1 - std::powf(1 - reaction.getProbability(), dt))
            continue;

        const auto& vVec = d1->getVelocity();
        const float v = MathUtils::abs(vVec);
        const sf::Vector2f n = vVec / v;

        // We will let the collision handling in the next time step take care of separation
        // But we can't have identical positions, so this ASSUMES that discs will be moved BEFORE the next collision
        // handling
        Disc product1(reaction.getProduct1());
        product1.setVelocity(v * sf::Vector2f{-n.y, n.x});

        Disc product2(reaction.getProduct2());
        product2.setVelocity(v * sf::Vector2f{n.y, -n.x});

        newDiscs.push_back(std::move(product1));
        newDiscs.push_back(std::move(product2));
        d1->markDestroyed();

        return;
    }

    return;
}