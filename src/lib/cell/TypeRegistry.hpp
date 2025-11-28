#ifndef C9F0AFE7_2B64_483A_9B0A_9B7D7DA9DEFC_HPP
#define C9F0AFE7_2B64_483A_9B0A_9B7D7DA9DEFC_HPP

#include "ExceptionWithLocation.hpp"
#include "Types.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace cell
{

template <typename ValueType> class TypeRegistry
{
public:
    using KeyType = std::uint16_t;

public:
    TypeRegistry() = default;
    TypeRegistry(const TypeRegistry&) = delete;
    TypeRegistry(TypeRegistry&&) = default;

    void setValues(std::vector<ValueType>&& values);
    const std::vector<ValueType>& getValues() const;

    KeyType getIDFor(const std::string& name) const;
    const ValueType& getByID(KeyType ID) const;

private:
    void buildNameIDMap(const std::vector<ValueType>& values);

private:
    std::vector<ValueType> values_;
    std::unordered_map<std::string, KeyType> nameIDMap_;
};

template <typename ValueType> inline void TypeRegistry<ValueType>::setValues(std::vector<ValueType>&& values)
{
    if (values.empty())
    {
        values_.clear();
        return;
    }

    const auto MaxCount = std::numeric_limits<KeyType>::max();
    if (values.size() > MaxCount)
        throw ExceptionWithLocation("Too many types: Registry only supports " + std::to_string(MaxCount) + ", but " +
                                    std::to_string(values.size()) + " were given");

    buildNameIDMap(values);

    values_ = std::move(values);
}

template <typename ValueType> inline const std::vector<ValueType>& TypeRegistry<ValueType>::getValues() const
{
    return values_;
}

template <typename ValueType>
inline TypeRegistry<ValueType>::KeyType TypeRegistry<ValueType>::getIDFor(const std::string& name) const
{
    auto iter = nameIDMap_.find(name);
    if (iter == nameIDMap_.end())
        throw ExceptionWithLocation("Couldn't find type with name \"" + name + "\" in registry");

    return iter->second;
}

template <typename ValueType> inline const ValueType& TypeRegistry<ValueType>::getByID(KeyType ID) const
{
#ifdef DEBUG
    if (static_cast<std::size_t>(ID) >= values_.size())
        throw ExceptionWithLocation("Index out of range: " + std::to_string(ID) + ", registry size is " +
                                    std::to_string(values_.size()));
#endif

    return values_[ID];
}

template <typename ValueType> inline void TypeRegistry<ValueType>::buildNameIDMap(const std::vector<ValueType>& values)
{
    std::unordered_map<std::string, KeyType> nameIDMap;

    for (KeyType i = 0; i < static_cast<KeyType>(values.size()); ++i)
    {
        if (nameIDMap.contains(values[i].getName()))
            throw ExceptionWithLocation("Can't build name map: Duplicate type name \"" + values[i].getName() + "\"");

        nameIDMap[values[i].getName()] = i;
    }

    nameIDMap_ = std::move(nameIDMap);
}

} // namespace cell
#endif /* C9F0AFE7_2B64_483A_9B0A_9B7D7DA9DEFC_HPP */
