#include "ReactionEngine.hpp"
#include "Disc.hpp"
#include "MathUtils.hpp"
#include "Reaction.hpp"
#include "ReactionTable.hpp"

namespace cell
{

ReactionEngine::ReactionEngine(const DiscTypeRegistry& discTypeRegistry, const ReactionTable& reactionTable)
    : discTypeRegistry_(discTypeRegistry)
{
    combineReactionsIntoSingleMaps(reactionTable);
}

Disc ReactionEngine::transformationReaction(Disc* educt, DiscTypeID productID) const
{
    Disc product(*educt);
    product.setType(productID);
    educt->markDestroyed();

    return product;
}

std::pair<Disc, Disc> ReactionEngine::decompositionReaction(Disc* educt, DiscTypeID product1ID,
                                                            DiscTypeID product2ID) const
{
    double v = mathutils::abs(educt->getVelocity());
    if (v == 0)
    {
        // If the disc is stationary and wants to split apart, we'll give it a random velocity to do so
        educt->setVelocity(
            Vector2d{mathutils::getRandomNumber<double>(-10, 10), mathutils::getRandomNumber<double>(-10, 10)});
        v = mathutils::abs(educt->getVelocity());
    }

    const Vector2d eductNormalizedVelocity = educt->getVelocity() / v;
    const Vector2d n{-eductNormalizedVelocity.y, eductNormalizedVelocity.x};

    Disc product1(*educt);
    Disc product2(product2ID);

    product1.setType(product1ID);
    product1.setVelocity(v * n);

    product2.setPosition(educt->getPosition());
    product2.setVelocity(-v * n);

    const auto R1 = discTypeRegistry_.getByID(product1.getTypeID()).getRadius();
    const auto R2 = discTypeRegistry_.getByID(product2.getTypeID()).getRadius();
    const auto overlap = R1 + R2 + 1e-6; // Discs at same position always have maximum overlap R1 + R2

    product1.move(0.5 * overlap * n);
    product2.move(-0.5 * overlap * n);

    educt->markDestroyed();

    return std::make_pair(std::move(product1), std::move(product2));
}

Disc ReactionEngine::combinationReaction(Disc* educt1, Disc* educt2, DiscTypeID productID) const
{
    const auto& resultType = discTypeRegistry_.getByID(productID);
    const auto& d1Type = discTypeRegistry_.getByID(educt1->getTypeID());
    const auto& d2Type = discTypeRegistry_.getByID(educt2->getTypeID());

    Disc newDisc(productID);
    newDisc.setVelocity((d1Type.getMass() * educt1->getVelocity() + d2Type.getMass() * educt2->getVelocity()) /
                        resultType.getMass());
    newDisc.setPosition((educt1->getPosition() + educt2->getPosition()) / 2.0);

    educt1->markDestroyed();
    educt2->markDestroyed();

    return newDisc;
}

std::pair<Disc, Disc> ReactionEngine::exchangeReaction(Disc* educt1, Disc* educt2, DiscTypeID product1ID,
                                                       DiscTypeID product2ID) const
{
    const auto* d1Type = &discTypeRegistry_.getByID(educt1->getTypeID());
    const auto* d2Type = &discTypeRegistry_.getByID(educt2->getTypeID());
    const auto* product1Type = &discTypeRegistry_.getByID(product1ID);
    const auto* product2Type = &discTypeRegistry_.getByID(product2ID);

    // Sort both product types and educt discs by radius
    // Now the smallest/largest disc gets the smallest/largest product type

    if (product1Type->getRadius() > product2Type->getRadius())
    {
        std::swap(product1Type, product2Type);
        std::swap(product1ID, product2ID);
    }
    if (d1Type->getRadius() > d2Type->getRadius())
    {
        std::swap(educt1, educt2);
        std::swap(d1Type, d2Type);
    }

    // Prefer the assignment that keeps
    // as many discs as possible with their original type.

    int leaveAsIs = (educt1->getTypeID() == product1ID) + (educt2->getTypeID() == product2ID);
    int swapAgain = (educt1->getTypeID() == product2ID) + (educt2->getTypeID() == product1ID);

    if (swapAgain > leaveAsIs)
    {
        std::swap(product1Type, product2Type);
        std::swap(product1ID, product2ID);
    }

    Disc product1(*educt1);
    Disc product2(*educt2);

    product1.scaleVelocity(std::sqrt(d1Type->getMass() / product1Type->getMass()));
    product1.setType(product1ID);

    product2.scaleVelocity(std::sqrt(d2Type->getMass() / product2Type->getMass()));
    product2.setType(product2ID);

    educt1->markDestroyed();
    educt2->markDestroyed();

    return std::make_pair(std::move(product1), std::move(product2));
}

void ReactionEngine::applyUnimolecularReactions(Disc& disc, double dt, std::vector<Disc>& newDiscs) const
{
    const Reaction* reaction = selectUnimolecularReaction(disc.getTypeID(), dt);
    if (!reaction)
        return;

    if (reaction->getType() == Reaction::Type::Transformation)
    {
        auto product = transformationReaction(&disc, reaction->getProduct1());
        newDiscs.push_back(std::move(product));
    }
    else
    {
        auto products = decompositionReaction(&disc, reaction->getProduct1(), reaction->getProduct2());
        newDiscs.push_back(std::move(products.first));
        newDiscs.push_back(std::move(products.second));
    }
}

void ReactionEngine::applyBimolecularReactions(const std::vector<CollisionDetector::Collision>& collisions,
                                               std::vector<Disc>& newDiscs) const
{
    for (const auto& collision : collisions)
    {
        if (collision.invalidatedByDestroyedDiscs())
            continue;

        const Reaction* reaction =
            selectBimolecularReaction(std::minmax(collision.disc->getTypeID(), collision.otherDisc->getTypeID()));
        if (!reaction)
            continue;

        if (reaction->getType() == Reaction::Type::Combination)
        {
            auto product = combinationReaction(collision.disc, collision.otherDisc, reaction->getProduct1());
            newDiscs.push_back(std::move(product));
        }
        else
        {
            auto products =
                exchangeReaction(collision.disc, collision.otherDisc, reaction->getProduct1(), reaction->getProduct2());
            newDiscs.push_back(std::move(products.first));
            newDiscs.push_back(std::move(products.second));
        }
    }
}

const Reaction* ReactionEngine::selectUnimolecularReaction(const DiscTypeID& key, double dt) const
{
    return selectReaction(
        unimolecularReactions_, key, [&](const Reaction& reaction)
        { return mathutils::getRandomNumber<double>(0, 1) <= 1 - std::pow(1 - reaction.getProbability(), dt); });
}

const Reaction* ReactionEngine::selectBimolecularReaction(const std::pair<DiscTypeID, DiscTypeID>& key) const
{
    return selectReaction(bimolecularReactions_, key, [](const Reaction& reaction)
                          { return mathutils::getRandomNumber<double>(0, 1) <= reaction.getProbability(); });
}

void ReactionEngine::combineReactionsIntoSingleMaps(const ReactionTable& reactionTable)
{
    for (const auto& table : {reactionTable.getTransformations(), reactionTable.getDecompositions()})
    {
        for (const auto& [educt, reactions] : table)
            unimolecularReactions_[educt].insert(unimolecularReactions_[educt].end(), reactions.begin(),
                                                 reactions.end());
    }

    for (const auto& table : {reactionTable.getCombinations(), reactionTable.getExchanges()})
    {
        for (const auto& [educts, reactions] : table)
            bimolecularReactions_[educts].insert(bimolecularReactions_[educts].end(), reactions.begin(),
                                                 reactions.end());
    }

    // Random shuffle all reactions to avoid a bias
    std::minstd_rand rng{std::random_device{}()};

    for (auto& [educt, reactions] : unimolecularReactions_)
        std::shuffle(reactions.begin(), reactions.end(), rng);

    for (auto& [educts, reactions] : bimolecularReactions_)
        std::shuffle(reactions.begin(), reactions.end(), rng);
}

} // namespace cell