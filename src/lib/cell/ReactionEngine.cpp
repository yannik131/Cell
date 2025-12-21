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

std::optional<Disc> ReactionEngine::transformationReaction(Disc* disc, double dt) const
{
    const Reaction* reaction = selectUnimolecularReaction(*transformations_, disc->getTypeID(), dt);
    if (!reaction)
        return {};

    Disc product(*disc);
    product.setType(reaction->getProduct1());
    disc->markDestroyed();

    return product;
}

std::optional<std::pair<Disc, Disc>> ReactionEngine::decompositionReaction(Disc* d1, double dt) const
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

    Disc product1(*d1);
    Disc product2(reaction->getProduct2());

    product1.setType(reaction->getProduct1());
    product1.setVelocity(v * n);

    product2.setPosition(d1->getPosition());
    product2.setVelocity(-v * n);

    const auto R1 = discTypeRegistry_.getByID(product1.getTypeID()).getRadius();
    const auto R2 = discTypeRegistry_.getByID(product2.getTypeID()).getRadius();
    const auto overlap = R1 + R2 + 1e-6; // Discs at same position always have maximum overlap R1 + R2

    product1.move(0.5 * overlap * n);
    product2.move(-0.5 * overlap * n);

    d1->markDestroyed();

    return std::make_pair(std::move(product1), std::move(product2));
}

std::optional<Disc> ReactionEngine::combinationReaction(Disc* d1, Disc* d2) const
{
    const Reaction* reaction =
        selectBimolecularReaction(*combinations_, std::make_pair(d1->getTypeID(), d2->getTypeID()));
    if (!reaction)
        return {};

    const auto& resultType = discTypeRegistry_.getByID(reaction->getProduct1());
    const auto& d1Type = discTypeRegistry_.getByID(d1->getTypeID());
    const auto& d2Type = discTypeRegistry_.getByID(d2->getTypeID());

    Disc newDisc(reaction->getProduct1());
    newDisc.setVelocity((d1Type.getMass() * d1->getVelocity() + d2Type.getMass() * d2->getVelocity()) /
                        resultType.getMass());
    newDisc.setPosition((d1->getPosition() + d2->getPosition()) / 2.0);

    d1->markDestroyed();
    d2->markDestroyed();

    return newDisc;
}

std::optional<std::pair<Disc, Disc>> ReactionEngine::exchangeReaction(Disc* d1, Disc* d2) const
{
    const Reaction* reaction = selectBimolecularReaction(*exchanges_, std::make_pair(d1->getTypeID(), d2->getTypeID()));
    if (!reaction)
        return {};

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

    // Prefer the assignment that keeps
    // as many discs as possible with their original type.

    int leaveAsIs = (d1->getTypeID() == product1TypeID) + (d2->getTypeID() == product2TypeID);
    int swapAgain = (d1->getTypeID() == product2TypeID) + (d2->getTypeID() == product1TypeID);

    if (swapAgain > leaveAsIs)
    {
        std::swap(product1Type, product2Type);
        std::swap(product1TypeID, product2TypeID);
    }

    Disc product1(*d1);
    Disc product2(*d2);

    product1.scaleVelocity(std::sqrt(d1Type->getMass() / product1Type->getMass()));
    product1.setType(product1TypeID);

    product2.scaleVelocity(std::sqrt(d2Type->getMass() / product2Type->getMass()));
    product2.setType(product2TypeID);

    d1->markDestroyed();
    d2->markDestroyed();

    return std::make_pair(std::move(product1), std::move(product2));
}

void ReactionEngine::applyUnimolecularReactions(Disc& disc, double dt, std::vector<Disc>& newDiscs) const
{
    // TODO random shuffle all reactions. For now we'll keep it simple

    if (auto product = transformationReaction(&disc, dt))
        newDiscs.push_back(std::move(*product));
    else if (auto products = decompositionReaction(&disc, dt))
    {
        newDiscs.push_back(std::move(products->first));
        newDiscs.push_back(std::move(products->second));
    }
}

void ReactionEngine::applyBimolecularReactions(const std::vector<CollisionDetector::Collision>& collisions,
                                               std::vector<Disc>& newDiscs) const
{
    for (const auto& collision : collisions)
    {
        if (collision.invalidatedByDestroyedDiscs())
            continue;

        if (auto product = combinationReaction(collision.disc, collision.otherDisc))
        {
            newDiscs.push_back(std::move(*product));
            continue;
        }
        else if (auto products = exchangeReaction(collision.disc, collision.otherDisc))
        {
            newDiscs.push_back(std::move(products->first));
            newDiscs.push_back(std::move(products->second));
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