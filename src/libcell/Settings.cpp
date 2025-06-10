#include "Settings.hpp"
#include "ExceptionWithLocation.hpp"

cell::SettingID::SettingID(const std::string& key)
    : key_(key)
{
}

const std::string& cell::SettingID::getKey() const
{
    return key_;
}

void cell::Settings::loadFromJson(const fs::path& jsonFile)
{
    std::ifstream in(jsonFile);
    if (!in)
        throw ExceptionWithLocation("Couldn't open file: " + jsonFile.string());

    in >> jsonData_;
}

void cell::Settings::saveAsJson(const fs::path& jsonFile)
{
    std::ofstream out(jsonFile);
    out << jsonData_.dump(4);
}
