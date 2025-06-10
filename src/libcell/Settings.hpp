#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "DiscType.hpp"
#include "Reaction.hpp"
#include "ReactionTable.hpp"

#include <SFML/System/Time.hpp>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace cell
{

class SettingID
{
public:
    SettingID(const std::string& key);

    const std::string& getKey() const;

private:
    std::string key_;
};

class Settings
{
public:
    template <typename T> T get(const SettingID& settingID) const
    {
        auto iter = jsonData_.find(settingID.getKey());
        if (iter == jsonData_.end())
            throw ExceptionWithLocation("No setting found for key " + settingID.getKey());

        return iter->at(settingID.getKey()).get<T>();
    }

    template <typename T> T getMin(const SettingID& settingID) const
    {
        const std::string minKey = settingID.getKey() + "__min";

        auto iter = jsonData_.find(minKey);
        if (iter == jsonData_.end())
            throw ExceptionWithLocation("No minimum found for key " + settingID.getKey());

        return iter->at(minKey).get<T>();
    }

    template <typename T> T getMax(const SettingID& settingID) const
    {
        const std::string maxKey = settingID.getKey() + "__max";

        auto iter = jsonData_.find(maxKey);
        if (iter == jsonData_.end())
            throw ExceptionWithLocation("No maximum found for key " + settingID.getKey());

        return iter->at(maxKey).get<T>();
    }

    template <typename T>
    void set(const SettingID& settingID, const T& value, std::optional<T> min, std::optional<T> max)
    {
        jsonData_[settingID.getKey()] = value;

        if (min.has_value())
            jsonData_[settingID.getKey() + "__min"] = *min;

        if (max.has_value())
            jsonData_[settingID.getKey() + "__max"] = *max;
    }

    void loadFromJson(const fs::path& jsonFile);

    void saveAsJson(const fs::path& jsonFile);

private:
    nlohmann::json jsonData_;
};

} // namespace cell

#endif /* SETTINGS_HPP */
