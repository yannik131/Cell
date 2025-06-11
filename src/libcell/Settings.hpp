#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "DiscType.hpp"
#include "Reaction.hpp"
#include "ReactionTable.hpp"

#include <SFML/System/Time.hpp>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <type_traits>

namespace fs = std::filesystem;

namespace cell
{

template <typename T> class SettingID
{
public:
    explicit SettingID(const std::string& key);

    const std::string& getKey() const;

private:
    std::string key_;
};

inline void to_json(nlohmann::json& j, const sf::Time& time)
{
    j = time.asMicroseconds();
}

inline void from_json(const nlohmann::json& j, sf::Time& time)
{
    time = sf::microseconds(j.get<long long>());
}

class Settings
{
public:
    template <typename T>
    void set(const SettingID<T>& settingID, const T& value, std::optional<T> min, std::optional<T> max)
    {
        jsonData_[settingID.getKey()] = value;

        if (min.has_value())
            jsonData_[settingID.getKey() + "__min"] = *min;

        if (max.has_value())
            jsonData_[settingID.getKey() + "__max"] = *max;
    }

    template <typename T> T get(const SettingID<T>& settingID) const
    {
        return jsonData_.at(settingID.getKey()).get<T>();
    }

    template <typename T> T getMin(const SettingID<T>& settingID) const
    {
        const std::string minKey = settingID.getKey() + "__min";
        return jsonData_.at(minKey).get<T>();
    }

    template <typename T> T getMax(const SettingID<T>& settingID) const
    {
        const std::string maxKey = settingID.getKey() + "__max";

        return jsonData_.at(maxKey).get<T>();
    }

    void loadFromJson(const fs::path& jsonFile);

    void saveAsJson(const fs::path& jsonFile);

private:
    nlohmann::json jsonData_;
};

} // namespace cell

#endif /* SETTINGS_HPP */
