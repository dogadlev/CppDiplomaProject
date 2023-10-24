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
            Config::configs.insert(std::make_pair<std::string, std::string>(tmp.substr(0, eqPos), tmp.substr(eqPos + 1, std::string::npos)));
        }
    }

    file.close();
}