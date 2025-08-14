#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

class UncopyableStuff
{
private:
    std::string information;

public:
    UncopyableStuff(const std::string& information)
        : information(information)
    {
    }
    UncopyableStuff(UncopyableStuff&&) = default;
    UncopyableStuff(const UncopyableStuff&) = delete;
    UncopyableStuff& operator=(const UncopyableStuff&) = delete;
    UncopyableStuff& operator=(UncopyableStuff&&) = default;

    const std::string& getInformation() const
    {
        return information;
    }
};

class Item
{
private:
    const UncopyableStuff* theStuff;

public:
    Item(const UncopyableStuff* stuff)
        : theStuff(stuff)
    {
    }

    const UncopyableStuff* getStuff() const
    {
        return theStuff;
    }
    void setStuff(const UncopyableStuff* stuff)
    {
        theStuff = stuff;
    }
};

namespace nlohmann
{
template <> struct adl_serializer<UncopyableStuff>
{
    static void to_json(json& j, const UncopyableStuff& u)
    {
        j = json{{"information", u.getInformation()}};
    }

    static UncopyableStuff from_json(const json& j)
    {
        return UncopyableStuff(j.at("information").get<std::string>());
    }
};
} // namespace nlohmann

int main()
{
    std::vector<UncopyableStuff> storage;
    json saved_storage;

    // First run
    {
        storage.emplace_back("Important information!");
        Item item(&storage.back());

        saved_storage = storage; // serialize the whole storage
        std::cout << "Serialized: " << saved_storage.dump() << "\n";
    }

    // Simulate app restart
    storage.clear();

    // Second run
    {
        // Restore storage first
        storage = saved_storage.get<std::vector<UncopyableStuff>>();

        // Now recreate the item using restored storage
        Item item(nullptr);
        item.setStuff(&storage.back());

        std::cout << "Restored item stuff: " << item.getStuff()->getInformation() << "\n";
    }
}
