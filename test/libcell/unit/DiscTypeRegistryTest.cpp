#include "DiscTypeRegistry.hpp"
#include "DiscType.hpp"

#include <gmock/gmock.h>

using namespace cell;
using namespace testing;

class ADiscTypeRegistry : public Test
{
protected:
    DiscTypeRegistry discTypeRegistry;
    std::vector<DiscType> discTypes;
};

TEST_F(ADiscTypeRegistry, GivesUsableIDsForDiscTypes)
{
    discTypes.emplace_back("A", sf::Color::Green, 1.0, 2.0);
    discTypeRegistry.setDiscTypes(std::move(discTypes));

    DiscTypeID ID = discTypeRegistry.getIDFor("A");
    const auto& discType = discTypeRegistry.getByID(ID);

    ASSERT_THAT(discType.getName(), Eq("A"));
}

TEST_F(ADiscTypeRegistry, DoesNotAllowDuplicatesByName)
{
    discTypes.emplace_back("A", sf::Color::Green, 1.0, 2.0);
    discTypes.emplace_back("A", sf::Color::Blue, 1.0, 2.0);

    ASSERT_ANY_THROW(discTypeRegistry.setDiscTypes(std::move(discTypes)););
}
