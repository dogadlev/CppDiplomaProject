#include "config.h"

Config::Config(const std::string &filename)
{
    Config::parse(filename);
}

std::string Config::getConfig(const std::string &key) const
{
    if(Config::configs.empty())
    {
        throw std::runtime_error("Error. Empty config file.");
    }
    
    return Config::configs.at(key);
}

void Config::parse(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("File open error.");
    }
    std::string tmp;
    while (!file.eof())
    {
        std::getline(file, tmp);
        std::size_t eqPos = tmp.find("=");
        if (eqPos != std::string::npos)
        {
            std::string key = tmp.substr(0, eqPos);
            std::string value = tmp.substr(eqPos + 1, std::string::npos);
            if (key == "url")
            {
                std::size_t pos = value.find("/");
                if (pos == std::string::npos)
                    throw std::runtime_error("Wrong URL.");
                Config::configs.insert(std::make_pair<std::string, std::string>("domain", value.substr(0, pos)));
                Config::configs.insert(std::make_pair<std::string, std::string>("query", value.substr(pos, std::string::npos)));
            }
            else
                Config::configs.insert(std::make_pair<std::string, std::string>(std::move(key), std::move(value)));
        }
    }

    file.close();
}