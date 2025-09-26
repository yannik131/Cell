#ifndef C9F0AFE7_2B64_483A_9B0A_9B7D7DA9DEFC_HPP
#define C9F0AFE7_2B64_483A_9B0A_9B7D7DA9DEFC_HPP

#include "Types.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace cell
{

template <typename KeyType, typename ValueType> class TypeRegistry
{
    static_assert(std::is_integral_v<KeyType>, "KeyType must be an integral type");

public:
    TypeRegistry() = default;
    TypeRegistry(const TypeRegistry&) = delete;
    TypeRegistry(TypeRegistry&&) = default;

    void setValues(std::vector<ValueType>&& values);

    KeyType getIDFor(const std::string& name) const;
    const ValueType& getByID(KeyType ID) const;

    auto getResolver() const;

private:
    void buildNameIDMap(const std::vector<ValueType>& values);

private:
    std::vector<ValueType> values_;
    std::unordered_map<std::string, KeyType> nameIDMap_;
};

template <typename KeyType, typename ValueType>
inline void TypeRegistry<KeyType, ValueType>::setValues(std::vector<ValueType>&& values)
{
    if (values.empty())
    {
        values_.clear();
        return;
    }

    const auto& MaxCount = std::numeric_limits<KeyType>::max();
    if (values.size() > MaxCount)
        throw ExceptionWithLocation("Too many types: Registry only supports " + std::to_string(MaxCount) + ", but " +
                                    std::to_string(values.size()) + " were given");

    buildNameIDMap(values);

    values_ = std::move(values);
}

template <typename KeyType, typename ValueType>
inline KeyType TypeRegistry<KeyType, ValueType>::getIDFor(const std::string& name) const
{
    auto iter = nameIDMap_.find(name);
    if (iter == nameIDMap_.end())
        throw ExceptionWithLocation("Couldn't find type with name \"" + name + "\" in registry");

    return iter->second;
}

template <typename KeyType, typename ValueType>
inline const ValueType& TypeRegistry<KeyType, ValueType>::getByID(KeyType ID) const
{
#ifdef DEBUG
    if (static_cast<std::size_t>(ID) >= values_.size())
        throw ExceptionWithLocation("Index out of range: " + std::to_string(ID) + ", registry size is " +
                                    std::to_string(values_.size()));
#endif

    return values_[ID];
}

template <typename KeyType, typename ValueType> inline auto TypeRegistry<KeyType, ValueType>::getResolver() const
{
    return [&](KeyType discTypeID) -> const ValueType& { return getByID(discTypeID); };
}

template <typename KeyType, typename ValueType>
inline void TypeRegistry<KeyType, ValueType>::buildNameIDMap(const std::vector<ValueType>& values)
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
