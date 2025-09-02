#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

// It can be assumed that each type in the app is unique by name
class Type
{
private:
    std::string information;

public:
    Type(const std::string& information)
        : information(information)
    {
    }
    Type(Type&&) = default;
    Type(const Type&) = delete;
    Type& operator=(const Type&) = delete;
    Type& operator=(Type&&) = default;

    const std::string& getInformation() const
    {
        return information;
    }
};

class Item
{
private:
    const Type* type1_;
    const Type* type2_;

public:
    Item(const Type* type1, const Type* type2)
        : type1_(type1)
        , type2_(type2)
    {
    }

    const Type* getType1() const
    {
        return type1_;
    }

    const Type* getType2() const
    {
        return type2_;
    }

    void setType1(const Type* type1)
    {
        type1_ = type1;
    }

    void setType2(const Type* type2)
    {
        type2_ = type2;
    }
};

namespace nlohmann
{
template <> struct adl_serializer<Type>
{
    static void to_json(json& j, const Type& u)
    {
        j = json{{"information", u.getInformation()}};
    }

    static Type from_json(const json& j)
    {
        return Type(j.at("information").get<std::string>());
    }
};

template <> struct adl_serializer<Item>
{
    static void to_json(json& j, const Item& item)
    {
        j = json{};
        if (item.getType1() != nullptr)
        {
            j["type1"] = item.getType1()->getInformation();
        }
        else
        {
            j["type1"] = nullptr;
        }

        if (item.getType2() != nullptr)
        {
            j["type2"] = item.getType2()->getInformation();
        }
        else
        {
            j["type2"] = nullptr;
        }
    }

    static Item from_json(const json& j)
    {
        // This creates an item with null pointers - they need to be set later
        return Item(nullptr, nullptr);
    }
};
} // namespace nlohmann

// Helper function to find a type by information string
const Type* findTypeByInformation(const std::vector<Type>& storage, const std::string& info)
{
    auto it =
        std::find_if(storage.begin(), storage.end(), [&info](const Type& t) { return t.getInformation() == info; });
    return (it != storage.end()) ? &(*it) : nullptr;
}

int main()
{
    std::vector<Type> storage;
    storage.emplace_back("black");
    storage.emplace_back("green");
    json saved_storage;
    json saved_item;

    // First run
    {
        Item item(nullptr, &storage[std::rand() % 2]);

        saved_storage = storage; // serialize the whole storage
        std::cout << "Serialized: " << saved_storage.dump() << "\n";

        // TODO: Serialize item somehow
        saved_item = item;
        std::cout << "Serialized item: " << saved_item.dump() << "\n";
    }

    // Simulate app restart
    storage.clear();

    {
        // Restore storage first
        storage = saved_storage.get<std::vector<Type>>();

        // TODO: Deserialize the item somehow with the correct types
        Item item = saved_item.get<Item>();

        // Restore the type pointers by looking up in the restored storage
        if (!saved_item["type1"].is_null())
        {
            std::string type1_info = saved_item["type1"].get<std::string>();
            item.setType1(findTypeByInformation(storage, type1_info));
        }

        if (!saved_item["type2"].is_null())
        {
            std::string type2_info = saved_item["type2"].get<std::string>();
            item.setType2(findTypeByInformation(storage, type2_info));
        }

        std::cout << "Restored item: " << item.getType2()->getInformation() << "\n";
    }
}