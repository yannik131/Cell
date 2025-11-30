#include "ReactionEngine.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"
#include "Reaction.hpp"

namespace cell
{

ReactionEngine::ReactionEngine(const DiscTypeRegistry& discTypeRegistry, const AbstractReactionTable& reactionTable)
    : discTypeRegistry_(discTypeRegistry)
    , transformations_(&reactionTable.getTransformations())
    , decompositions_(&reactionTable.getDecompositions())
    , combinations_(&reactionTable.getCombinations())
    , exchanges_(&reactionTable.getExchanges())
{
}

bool ReactionEngine::transformationReaction(Disc* disc, double dt) const
{
    const Reaction* reaction = selectUnimolecularReaction(*transformations_, disc->getTypeID(), dt);
    if (!reaction)
        return false;

    disc->setType(reaction->getProduct1());

    return true;
}

std::optional<Disc> ReactionEngine::decompositionReaction(Disc* d1, double dt) const
{
    const Reaction* reaction = selectUnimolecularReaction(*decompositions_, d1->getTypeID(), dt);
    if (!reaction)
        return {};

    double v = mathutils::abs(d1->getVelocity());
    if (v == 0)
    {
        // If the disc is stationary and wants to split apart, we'll give it a random velocity to do so
        d1->setVelocity(
            sf::Vector2d{mathutils::getRandomNumber<double>(-10, 10), mathutils::getRandomNumber<double>(-10, 10)});
        v = mathutils::abs(d1->getVelocity());
    }

    const sf::Vector2d eductNormalizedVelocity = d1->getVelocity() / v;
    const sf::Vector2d n{-eductNormalizedVelocity.y, eductNormalizedVelocity.x};

    d1->setType(reaction->getProduct1());
    d1->setVelocity(v * n);

    Disc product2(reaction->getProduct2());
    product2.setPosition(d1->getPosition());
    product2.setVelocity(-v * n);

    const auto R1 = discTypeRegistry_.getByID(d1->getTypeID()).getRadius();
    const auto R2 = discTypeRegistry_.getByID(product2.getTypeID()).getRadius();
    const auto overlap = R1 + R2 + 1e-6; // Discs at same position always have maximum overlap R1 + R2

    d1->move(0.5 * overlap * n);
    product2.move(-0.5 * overlap * n);

    return product2;
}

bool ReactionEngine::combinationReaction(Disc* d1, Disc* d2) const
{
    const Reaction* reaction =
        selectBimolecularReaction(*combinations_, std::make_pair(d1->getTypeID(), d2->getTypeID()));
    if (!reaction)
        return false;

    const auto& resultType = discTypeRegistry_.getByID(reaction->getProduct1());
    const auto* d1Type = &discTypeRegistry_.getByID(d1->getTypeID());
    const auto* d2Type = &discTypeRegistry_.getByID(d2->getTypeID());

    // For reactions of type A + B -> C, we keep the one closer in size to C and destroy the other
    if (std::abs(resultType.getRadius() - d1Type->getRadius()) > std::abs(resultType.getRadius() - d2Type->getRadius()))
    {
        std::swap(d1, d2);
        std::swap(d1Type, d2Type);
    }

    d1->setVelocity((d1Type->getMass() * d1->getVelocity() + d2Type->getMass() * d2->getVelocity()) /
                    resultType.getMass());
    d1->setType(reaction->getProduct1());

    d2->markDestroyed();

    return true;
}

bool ReactionEngine::exchangeReaction(Disc* d1, Disc* d2) const
{
    const Reaction* reaction = selectBimolecularReaction(*exchanges_, std::make_pair(d1->getTypeID(), d2->getTypeID()));
    if (!reaction)
        return false;

    auto product1TypeID = reaction->getProduct1();
    auto product2TypeID = reaction->getProduct2();

    const auto* d1Type = &discTypeRegistry_.getByID(d1->getTypeID());
    const auto* d2Type = &discTypeRegistry_.getByID(d2->getTypeID());
    const auto* product1Type = &discTypeRegistry_.getByID(product1TypeID);
    const auto* product2Type = &discTypeRegistry_.getByID(product2TypeID);

    // Sort both product types and educt discs by radius
    // Now the smallest/largest disc gets the smallest/largest product type

    if (product1Type->getRadius() > product2Type->getRadius())
    {
        std::swap(product1Type, product2Type);
        std::swap(product1TypeID, product2TypeID);
    }
    if (d1Type->getRadius() > d2Type->getRadius())
    {
        std::swap(d1, d2);
        std::swap(d1Type, d2Type);
    }

    d1->scaleVelocity(std::sqrt(d1Type->getMass() / product1Type->getMass()));
    d1->setType(product1TypeID);

    d2->scaleVelocity(std::sqrt(d2Type->getMass() / product2Type->getMass()));
    d2->setType(product2TypeID);

    return true;
}

std::optional<Disc> ReactionEngine::applyUnimolecularReactions(Disc& disc, double dt) const
{
    // TODO random shuffle all reactions
    if (transformationReaction(&disc, dt))
        return {};

    return decompositionReaction(&disc, dt);
}

void ReactionEngine::applyBimolecularReactions(CollisionDetector::DetectedCollisions& detectedCollisions) const
{
    for (auto collisionType :
         {CollisionDetector::CollisionType::DiscDisc, CollisionDetector::CollisionType::DiscIntrudingDisc})
    {
        auto indexes = detectedCollisions.indexes.find(collisionType);
        if (indexes == detectedCollisions.indexes.end())
            continue;

        for (std::size_t index : indexes->second)
        {
            const auto& collision = detectedCollisions.collisions[index];
            if (collision.isInvalidatedByDestroyedDisc())
                continue;

            if (combinationReaction(collision.disc, collision.otherDisc))
                continue;
            else
                exchangeReaction(collision.disc, collision.otherDisc);
        }
    }
}

const Reaction* ReactionEngine::selectUnimolecularReaction(const SingleLookupMap& map, const DiscTypeID& key,
                                                           double dt) const
{
    return selectReaction(
        map, key, [&](const Reaction& reaction)
        { return mathutils::getRandomNumber<double>(0, 1) <= 1 - std::pow(1 - reaction.getProbability(), dt); });
}

const Reaction* ReactionEngine::selectBimolecularReaction(const PairLookupMap& map,
                                                          const std::pair<DiscTypeID, DiscTypeID>& key) const
{
    return selectReaction(map, key, [](const Reaction& reaction)
                          { return mathutils::getRandomNumber<double>(0, 1) <= reaction.getProbability(); });
}

} // namespace cell