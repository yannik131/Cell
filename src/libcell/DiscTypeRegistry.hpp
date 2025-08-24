#ifndef DISCTYPEREGISTRY_HPP
#define DISCTYPEREGISTRY_HPP

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

    void setDiscTypes(std::vector<DiscType>&& discTypes);

    DiscTypeID getIDFor(const std::string& discTypeName) const;
    const DiscType& getByID(DiscTypeID ID) const;

private:
    void buildNameIDMap(const std::vector<DiscType>& discTypes);

private:
    std::vector<DiscType> discTypes_;
    std::unordered_map<std::string, DiscTypeID> nameIDMap_;
};

} // namespace cell

#endif /* DISCTYPEREGISTRY_HPP */
