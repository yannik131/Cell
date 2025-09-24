#ifndef C54879E6_097A_4A86_A254_A24E632B463A_HPP
#define C54879E6_097A_4A86_A254_A24E632B463A_HPP

#include "Types.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace cell
{

class DiscType;

class DiscTypeRegistry
{
public:
    DiscTypeRegistry() = default;
    DiscTypeRegistry(const DiscTypeRegistry&) = delete;
    DiscTypeRegistry(DiscTypeRegistry&&) = default;

    void setDiscTypes(std::vector<DiscType>&& discTypes);

    DiscTypeID getIDFor(const std::string& discTypeName) const;
    const DiscType& getByID(DiscTypeID ID) const;

    double getMaxRadius() const;

    DiscTypeResolver getDiscTypeResolver() const;

private:
    void buildNameIDMap(const std::vector<DiscType>& discTypes);

private:
    std::vector<DiscType> discTypes_;
    std::unordered_map<std::string, DiscTypeID> nameIDMap_;
    double maxRadius_ = 0;
};

} // namespace cell

#endif /* C54879E6_097A_4A86_A254_A24E632B463A_HPP */
